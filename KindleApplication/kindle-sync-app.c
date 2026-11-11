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
#include <curl/curl.h>
#include <string.h>

const char *URL = "http://0.0.0.0:8080/message";

struct Response {
    char text[4096];
    size_t length;
};

static size_t receive_text(char *data, size_t size, size_t count, void *userdata){
    struct Response *response = userdata;
    //taille encore disponible pour recevoir
    size_t available = sizeof(response->text) - 1 - response->length;
    //verifier capacitée
    if (size != 0 && count > available / size)
        return 0; //Stop la requête

    size_t bytes = size * count;
    if (bytes == 0){
        return 0;
    }

    memcpy(response->text + response->length, data, bytes);
    response->length += bytes;
    response->text[response->length] = '\0';

    return bytes;
}

static int fetchAPI(struct Response *response){
    CURL *curl;
    CURLcode result;

    //inits request
    result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
        return (int)result;

    //fetch
    curl = curl_easy_init();
    if (!curl){
        fprintf(stderr, "Création du client imppossible\n");
        curl_global_cleanup();
        return 1;
    }
    if (curl){
        //set options
        result = curl_easy_setopt(curl, CURLOPT_URL, URL);

        if (result == CURLE_OK)
            result = curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        if (result == CURLE_OK)
            result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_text);

        if (result == CURLE_OK)
            result = curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        if (result == CURLE_OK)
            result = curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        //perform
        if (result == CURLE_OK)
            result = curl_easy_perform(curl);

        //errors

        if (result != CURLE_OK){
            fprintf(stderr, "curl_easy_perform failed %s \n", curl_easy_strerror(result));
        }else {
            //truc à faire
        }
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return (int)result;
}

static int wait_for_touch(int fbfd, FBInkConfig *config){
    struct Response shown = {0};
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
    bool refreshed = true;
    struct pollfd input = {.fd = fd, .events = POLLIN};

    //écoute et vérif
    for (;;){
        //lance un timer de 30scd à chaque input
        if (refreshed){
            struct Response next = {0};
            if (fetchAPI(&next) == 0 && strcmp(next.text, shown.text) != 0){
                if (fbink_print(fbfd, next.text, config) < 0){
                    fprintf(stderr, "Erreur d'affichage GBInk\n");
                    break;
                }
                shown = next;
            }
            refreshed = false;
        }

        int ready = poll(&input, 1, 5000);

        if (ready == 0){
            refreshed = true;
            continue;
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
        .is_cleared = true,
        .is_quiet = true
    };

    int fd = fbink_open();

    if (fd < 0){
        fprintf(stderr, "Ouverture du framebuffer impossible : %d\n", fd);
        return 1;
    }

    //affichage du buffer sur l'écran
    int result = fbink_init(fd, &config);

    if (result < 0){
        fprintf(stderr, "Erreur FBInk : %d\n", result);
    }else{
        result = wait_for_touch(fd, &config);
    }

    int close_result = fbink_close(fd);
    if (close_result < 0)
        fprintf(stderr, "Erreur de fermeture FBInk %d\n", close_result);
    return (result < 0 || close_result < 0) ? 1 : 0;
}
