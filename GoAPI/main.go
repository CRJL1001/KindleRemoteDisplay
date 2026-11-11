package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"sync"
	"time"
)

type Content struct {
	Text string `json:"text"`
}

var content = "Bonjour depuis l'API !"
var mu sync.RWMutex

const ADRESS = "0.0.0.0:8080"

func main(){
	mux := http.NewServeMux()
	mux.HandleFunc("GET /message", handler)
	fs := http.FileServer(http.Dir("GoAPI/static"))
	mux.Handle("GET /static/", http.StripPrefix("/static/", fs))
	mux.HandleFunc("GET /{$}", indexhandler)
	mux.HandleFunc("POST /edit", editHandler)

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
	mu.RLock()
	current_content := content
	mu.RUnlock()
	if _, err := fmt.Fprintln(w, current_content); err != nil {
		log.Printf("Erreur de reponse :  %v", err)
	}
}

func indexhandler(w http.ResponseWriter, r *http.Request){
	http.ServeFile(w, r, "GoAPI/public/index.html")
}

func editHandler(w http.ResponseWriter, r *http.Request){
	log.Printf("%s %s depuis %s", r.Method, r.URL.Path, r.RemoteAddr)
	var payload Content
	if err := json.NewDecoder(r.Body).Decode(&payload); err != nil{
		http.Error(w, "Invalide request payload", http.StatusBadRequest)
		return
	}
	defer r.Body.Close()

	mu.Lock()
	content = payload.Text
	mu.Unlock()

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(http.StatusCreated)
	json.NewEncoder(w).Encode(payload)
}
