#include <stdio.h>
#include "fbink.h"
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>

static int wait_for_touch(void){
    //init tactile
    int fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (fd < 0){
        perror("Initialisation du tactile");
       return -1;
    }

    //Tactile input only for this programme
    if (ioctl(fd, EVIOCGRAB, 1) < 0){
        perror("Réservation du tactile");
        close(fd);
        return -1;
    }

    int result = -1;
    bool pressed = false;
    struct pollfd input = {.fd = fd, .events = POLLIN};

    //écoute et vérif
    for (;;){
        //lance un timer de 30scd à chaque input
        int ready = poll(&input, 1, 30000);

        if (ready == 0){
            fprintf(stderr, "Délais sans événements dépassés\n");
            break;
        }

        if (ready < 0){
            if (errno == EINTR)
                continue;
            perror("erreur poll");
           break;
        }

        //détection du périphérique d'input
        if (input.revents & (POLLERR | POLLHUP | POLLNVAL)){
            fprintf(stderr, "Périphériques tactiles indisponibles");
            break;
        }

        struct input_event event;
        ssize_t count = read(fd, &event, sizeof(event));

        //count = nb octets de l'événement
        if (count < 0){
            if (errno == EINTR || errno == EAGAIN)
                continue;
            perror("Lecture tactile");
            break;
        }

        if (count != sizeof(event)){
            fprintf(stderr, "Evenement tactile incomplet");
            break;
        }

        //verif événement perdu ?
        if (event.type == EV_SYN && event.code == SYN_DROPPED){
            fprintf(stderr, "événements tactiles perdus\n");
            break;
        }

        //détecter qu'un seul doigt touche et se retire
        if (event.type == EV_ABS && event.code == ABS_MT_TRACKING_ID){
            //si touché
            if (event.value >= 0){
                pressed = true;
            }else if (event.value == -1 && pressed){//si pas de contact et déjà pressé
                result = 0; //retour correcte
                break;
            }
        }
    }
    //fermeture de l'écoute et retour
    close(fd);
    return result;
}

int main(void){
    //init affichage
    FBInkConfig config = {
        .fontmult = 3,
        .is_centered = true,
        .is_halfway = true,
        .is_quiet = true
    };

    int fd = fbink_open();

    if (fd < 0){
        fprintf(stderr, "Ouverture du framebuffer impossible : %d\n", fd);
        return 1;
    }

    //affichage du buffer sur l'écran
    int result = fbink_init(fd, &config);

    if (result >= 0){
        result = fbink_print(fd, "Bonjour !", &config);
    }

    if (result < 0){
        fprintf(stderr, "Erreur FBInk : %d\n", result);
    }else{
        result = wait_for_touch();
    }

    int close_result = fbink_close(fd);
    if (close_result < 0)
        fprintf(stderr, "Erreur de fermeture FBInk %d\n", close_result);
    return (result < 0 || close_result < 0) ? 1 : 0;
}
