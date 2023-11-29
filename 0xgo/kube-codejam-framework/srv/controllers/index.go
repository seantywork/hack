package controllers

import (
	"encoding/json"

	"github.com/gin-gonic/contrib/sessions"
	"github.com/gin-gonic/gin"

	"0xgo/kube-codejam-framework/adm/utils"
	"0xgo/kube-codejam-framework/srv/models"
)

func Index(c *gin.Context) {

	c.HTML(200, "index.html", gin.H{
		"title": "INDEX",
	})

}

func RegisterGoogleOauth(c *gin.Context) {

	email := c.PostForm("code_owner")

	if email == "" {
		utils.EventLogger("oauth google: email")
		c.Data(400, "text/plain; charset=utf-8", []byte("bad request"))
		return
	}

	lang := c.PostForm("language")

	if lang == "" {
		utils.EventLogger("oauth google: lang")
		c.Data(400, "text/plain; charset=utf-8", []byte("bad request"))
		return
	}

	version := c.PostForm("version")

	if version == "" {
		utils.EventLogger("oauth google: version")
		c.Data(400, "text/plain; charset=utf-8", []byte("bad request"))
		return
	}

	code := c.PostForm("code")

	if code == "" {
		utils.EventLogger("oauth google: code")
		c.Data(400, "text/plain; charset=utf-8", []byte("bad request"))
		return
	}

	input := c.PostForm("input")

	if input == "" {
		utils.EventLogger("oauth google: input")
		c.Data(400, "text/plain; charset=utf-8", []byte("bad request"))
		return
	}

	err := models.CheckRegistryAvailability(email)

	if err != nil {

		utils.EventLogger("oauth google:" + err.Error())

		c.Data(500, "text/plain; charset=utf-8", []byte("request unavailable"))
		return
	}

	err = models.RegisterSubmitRecord(email, lang, version, code, input)

	if err != nil {

		utils.EventLogger("oauth google:" + err.Error())

		c.Data(500, "text/plain; charset=utf-8", []byte("failed to register"))
		return

	}

	oauth_state := GenerateStateOauthCookie(c)

	u := GoogleOauthConfig.AuthCodeURL(oauth_state)

	c.Redirect(302, u)

}

func RunGoogleOauth(c *gin.Context) {

	session := sessions.Default(c)

	var session_id string

	v := session.Get("SID")

	if v == nil {

		utils.EventLogger("access auth: session id not found")
		c.Data(403, "text/plain; charset=utf-8", []byte("illegal access"))
		return

	} else {
		session_id = v.(string)
	}

	state := c.Request.FormValue("state")

	if state == "" {
		utils.EventLogger("access auth: state not found")
		c.Data(403, "text/plain; charset=utf-8", []byte("illegal request"))
		return
	}

	if state != session_id {
		utils.EventLogger("access auth: value not matching")
		c.Redirect(302, "/run-payload")
		return
	}

	data, err := GetUserDataFromGoogle(c.Request.FormValue("code"))
	if err != nil {
		utils.EventLogger("access auth: " + err.Error())
		c.Redirect(302, "/run-payload")
		return
	}

	var oauth_struct OAuthStruct

	err = json.Unmarshal(data, &oauth_struct)

	if err != nil {
		utils.EventLogger("access auth: " + err.Error())
		c.Redirect(302, "/run-payload")
		return
	}

	if !oauth_struct.VERIFIED_EMAIL {
		utils.EventLogger("access auth: email not verified")
		c.Redirect(302, "/run-payload")
		return
	}

	email := oauth_struct.EMAIL

	rand_hex, _ := utils.RandomHex(16)

	err = models.RegisterVerificationFlagAndOwner(email, rand_hex)

	if err != nil {
		utils.EventLogger("access auth: " + err.Error())
		c.Redirect(302, "/run-payload")
		return
	}

	c.HTML(200, "run.html", gin.H{
		"code_owner": rand_hex,
	})

	return
}
