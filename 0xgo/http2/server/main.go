package main

import (
	"log"
	"net/http"
)

func main() {

	http.HandleFunc("/", handle)

	log.Printf("Serving on https://0.0.0.0:8000")
	log.Fatal(http.ListenAndServeTLS(":8000", "certs/server.pem", "certs/server.key", nil))
}

func handle(w http.ResponseWriter, r *http.Request) {

	log.Printf("Got connection: %s", r.Proto)

	w.Write([]byte("Hello"))
}
