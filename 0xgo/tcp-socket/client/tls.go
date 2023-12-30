package main

import (
	"crypto/tls"
	"crypto/x509"
	"fmt"
	"io"
	"log"
	"os"
)

func TlsConnection() {

	certpool := x509.NewCertPool()

	file_b, err := os.ReadFile("certs/ca.pem")

	/*
		ca_pem := new(bytes.Buffer)

		pem.Encode(ca_pem, &pem.Block{
			Type:  "CERTIFICATE",
			Bytes: file_b,
		})
	*/
	certpool.AppendCertsFromPEM(file_b)

	config := tls.Config{RootCAs: certpool}
	conn, err := tls.Dial("tcp", "localhost:8080", &config)
	if err != nil {
		log.Fatalf("client: dial: %s", err)
	}
	defer conn.Close()
	log.Println("client: connected to: ", conn.RemoteAddr())

	state := conn.ConnectionState()
	for _, v := range state.PeerCertificates {
		fmt.Println(x509.MarshalPKIXPublicKey(v.PublicKey))
		fmt.Println(v.Subject)
	}
	log.Println("client: handshake: ", state.HandshakeComplete)
	log.Println("client: mutual: ", state.NegotiatedProtocolIsMutual)

	message := "Hello\n"
	n, err := io.WriteString(conn, message)
	if err != nil {
		log.Fatalf("client: write: %s", err)
	}
	log.Printf("client: wrote %q (%d bytes)", message, n)

	reply := make([]byte, 256)
	n, err = conn.Read(reply)
	log.Printf("client: read %q (%d bytes)", string(reply[:n]), n)
	log.Print("client: exiting")
}
