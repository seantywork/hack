package main

import (
	"github.com/seantywork/0xgo/kube-codejam/adm/utils"

	"github.com/gin-gonic/contrib/sessions"
	"github.com/gin-gonic/gin"

	"github.com/seantywork/0xgo/kube-codejam/srv/models"
	"github.com/seantywork/0xgo/kube-codejam/srv/routers"
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
