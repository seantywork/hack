package resource

import (
	"encoding/json"
	"os"
)

type ConfigJSON map[string]string

func GetConfig() ConfigJSON {

	var cj ConfigJSON

	file_byte, err := os.ReadFile("config.json")

	if err != nil {
		panic(err)
	}

	err = json.Unmarshal(file_byte, &cj)

	if err != nil {
		panic(err)
	}

	return cj

}
