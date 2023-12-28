package utils

import (
	"crypto/rand"
	"encoding/hex"
	"log"
	"os"
	"time"
)

var L = log.New(os.Stdout, "", 0)

func EventLogger(msg string) {
	L.SetPrefix(time.Now().UTC().Format("2006-01-02 15:04:05.000") + " [INFO] ")
	L.Print(msg)
}

func RandomHex(n int) (string, error) {
	bytes := make([]byte, n)
	if _, err := rand.Read(bytes); err != nil {
		return "", err
	}
	return hex.EncodeToString(bytes), nil
}
