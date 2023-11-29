package controllers

import (
	"context"
	"crypto/rand"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"os"

	"github.com/gin-gonic/contrib/sessions"
	"github.com/gin-gonic/gin"
	"golang.org/x/oauth2"
	"golang.org/x/oauth2/google"
)

const OauthGoogleUrlAPI = "https://www.googleapis.com/oauth2/v2/userinfo?access_token="

var CONFIG_JSON = GetConfig()

type OAuthStruct struct {
	ID             string `json:"id"`
	EMAIL          string `json:"email"`
	VERIFIED_EMAIL bool   `json:"verified_email"`
	PICTURE        string `json:"picture"`
}

var GoogleOauthConfig = GenerateGoogleOauthConfig()

func GenerateGoogleOauthConfig() *oauth2.Config {

	google_oauth_config := &oauth2.Config{
		ClientID:     CONFIG_JSON["GOOGLE_OAUTH_CLIENT_ID"],
		ClientSecret: CONFIG_JSON["GOOGLE_OAUTH_CLIENT_SECRET"],
		Scopes:       []string{"https://www.googleapis.com/auth/userinfo.email"},
		Endpoint:     google.Endpoint,
	}

	if CONFIG_JSON["DEBUG"] == "REMOTE" {

		google_oauth_config.RedirectURL = CONFIG_JSON["REDIRECT_URL_REMOTE"]

	} else if CONFIG_JSON["DEBUG"] == "LOCAL" {

		google_oauth_config.RedirectURL = CONFIG_JSON["REDIRECT_URL_LOCAL"]

	} else {

		google_oauth_config.RedirectURL = CONFIG_JSON["REDIRECT_URL"]
	}

	return google_oauth_config

}

func GenerateStateOauthCookie(c *gin.Context) string {

	b := make([]byte, 16)
	rand.Read(b)

	state := base64.URLEncoding.EncodeToString(b)

	session := sessions.Default(c)

	session.Set("SID", state)
	session.Save()

	return state
}

func GetUserDataFromGoogle(code string) ([]byte, error) {

	token, err := GoogleOauthConfig.Exchange(context.Background(), code)
	if err != nil {
		return nil, fmt.Errorf("code exchange wrong: %s", err.Error())
	}
	response, err := http.Get(OauthGoogleUrlAPI + token.AccessToken)
	if err != nil {
		return nil, fmt.Errorf("failed getting user info: %s", err.Error())
	}
	defer response.Body.Close()
	contents, err := ioutil.ReadAll(response.Body)
	if err != nil {
		return nil, fmt.Errorf("failed read response: %s", err.Error())
	}
	return contents, nil
}

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
