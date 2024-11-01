package main

import (
	"fmt"
	"net"
	"os"
)

func main() {

	mode := os.Args[1]

	if mode == "net" {
		ln, err := net.Listen("tcp", "0.0.0.0:8080")
		if err != nil {
			fmt.Println(err)
			return
		}

		for {
			conn, err := ln.Accept()
			if err != nil {
				fmt.Println(err)
				continue
			}

			go handleConnection(conn)
		}
	} else if mode == "tls" {

		TlsListen()

	}

}

func handleConnection(conn net.Conn) {

	defer conn.Close()

	buf := make([]byte, 1024)
	_, err := conn.Read(buf)
	if err != nil {
		fmt.Println(err)
		return
	}

	fmt.Printf("Received: %s", buf)
}
