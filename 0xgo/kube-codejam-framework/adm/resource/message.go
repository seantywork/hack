package resource

type FrontRequest struct {
	CODE_OWNER string `json:"code_owner"`
	LANGUAGE   string `json:"language"`
	VERSION    string `json:"version"`
	CODE       string `json:"code"`
	INPUT      string `json:"input"`
}

type FrontResponse struct {
	STATUS   string `json:"status"`
	MESSAGE  string `json:"message"`
	RESOURCE string `json:"resource"`
	STDOUT   string `json:"stdout"`
	STDERR   string `json:"stderr"`
}
