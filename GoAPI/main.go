package main

import (
	"fmt"
	"log"
	"net/http"
	"time"
)

const CONTENT = "Bonjour depuis le PC !"
const ADRESS = "0.0.0.0:8080"

func main(){
	mux := http.NewServeMux()
	mux.HandleFunc("GET /message", handler)

	server := &http.Server{
		Addr: ADRESS,
		Handler: mux,
		ReadHeaderTimeout: 5 * time.Second,
		WriteTimeout: 5 * time.Second,
		IdleTimeout: 1 * time.Minute,
	}

	log.Println("Serveur en écoute sur le port 8080")
	log.Fatal(server.ListenAndServe())
}

func handler(w http.ResponseWriter, r *http.Request){
	log.Printf("%s %s depuis %s", r.Method, r.URL.Path, r.RemoteAddr)
	w.Header().Set("Content-type", "text/plain; charset=utf-8")
	if _, err := fmt.Fprintln(w, CONTENT); err != nil {
		log.Printf("Erreur de reponse :  %v", err)
	}
}
