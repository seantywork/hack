package main

import (
	"fmt"
	"net"
	"os"
)

func main() {

	mode := os.Args[1]

	if mode == "net" {
		conn, err := net.Dial("tcp", "localhost:8080")
		if err != nil {
			fmt.Println(err)
			return
		}

		_, err = conn.Write([]byte("Hello, server!"))
		if err != nil {
			fmt.Println(err)
			return
		}

		conn.Close()

	} else if mode == "tls" {

		TlsConnection()
	}

}
