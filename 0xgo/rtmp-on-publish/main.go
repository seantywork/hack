package main

import (
	"log"
	"net/http"
	"os"
)

var AUTH_ADDRESS string = "localhost"
var AUTH_PORT string = "5000"

func isValidStreamKey(p_user string) bool {

	retcode := false

	// check DB or something

	// for now here, just checking an entry in a bogus file

	file_b, err := os.ReadFile("bogus")

	if err != nil {
		log.Println("failed to read bogus file")
		return false
	}

	file_str := string(file_b)

	if p_user != file_str {
		log.Printf("entry user doesn't match : %s", p_user)
		return false

	}

	retcode = true

	return retcode
}

func HandleRtmpAuthentication(w http.ResponseWriter, r *http.Request) {

	p_user := r.URL.Query().Get("puser")
	if p_user == "" {
		log.Println("puser field is missing")
		http.Error(w, "puser field is missing", http.StatusBadRequest)
		return
	}

	if !isValidStreamKey(p_user) {
		log.Println("stream request denied")
		w.WriteHeader(http.StatusForbidden)
		return
	}

	log.Println("stream request accepted")
	w.WriteHeader(http.StatusOK)
	return
}

func RtmpAuthServer() *http.Server {

	http.HandleFunc("/auth", HandleRtmpAuthentication)
	log.Printf("Rtmp Auth server listening at %v", AUTH_PORT)
	err := http.ListenAndServe(AUTH_ADDRESS+":"+AUTH_PORT, nil)
	if err != nil {
		log.Fatalf("failed to run Rtmp Auth server: %v", err)
	}
	return &http.Server{}
}

func main() {
	RtmpAuthServer()
}
