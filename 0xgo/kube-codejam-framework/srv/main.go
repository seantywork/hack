package main

import (
	"0xgo/kube-codejam-framework/adm/utils"

	"github.com/gin-gonic/contrib/sessions"
	"github.com/gin-gonic/gin"

	"0xgo/kube-codejam-framework/srv/models"
	"0xgo/kube-codejam-framework/srv/routers"
)

func main() {

	utils.EventLogger("Server starting...")

	models.InitDBConn()

	srv := gin.Default()

	store := sessions.NewCookieStore([]byte("secret"))

	srv.Use(sessions.Sessions("run-payload-session", store))

	srv.LoadHTMLGlob("view/*")

	srv.Static("/static", "./static")

	routers.Init(srv)

	utils.EventLogger("Server started")

	srv.Run("0.0.0.0:15884")

}
