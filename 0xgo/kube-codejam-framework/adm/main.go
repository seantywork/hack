package main

import (
	"flag"
	"fmt"
	"log"
	"os/exec"

	_ "log"
	"net/http"
	_ "net/http"
	_ "os"
	_ "time"

	"0xgo/kube-codejam-framework/adm/querydb"

	"github.com/gorilla/websocket"
)

var ADDR = flag.String("addr", "0.0.0.0:15888", "service address")

var UPGRADER = websocket.Upgrader{} // use default options

var USERCONTENT_DIR = "uc/"

func main() {

	querydb.InitDBConn()

	cmd := exec.Command("mkdir", "-p", "uc")

	_, err := cmd.Output()

	if err != nil {
		fmt.Println(err.Error())
		return
	}

	flag.Parse()
	log.SetFlags(0)

	http.HandleFunc("/run-payload-adm", RunPayloadAdm)
	log.Fatal(http.ListenAndServe(*ADDR, nil))

}
