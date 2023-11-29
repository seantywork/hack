package build

import (
	"fmt"
	"os/exec"
)

func CompileC() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.c", "/usercontent/u.c")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("gcc", "-o", "/usercontent/c", "/usercontent/u.c")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/c")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/c")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | /usercontent/c"

	return out_loc, nil
}

func CompileCC() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.cc", "/usercontent/u.cc")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("g++", "-o", "/usercontent/cc", "/usercontent/u.cc")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/cc")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/cc")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | /usercontent/cc"

	return out_loc, nil
}

func CompileGO() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.go", "/usercontent/u.go")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("go", "-o", "/usercontent/go", "/usercontent/u.go")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/go")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/go")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | /usercontent/go"

	return out_loc, nil
}

func CompileJAVA() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.java", "/usercontent/u.java")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("java", "-o", "/usercontent/go", "/usercontent/u.go")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/java")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/java")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | /usercontent/java"

	return out_loc, nil
}

func CompilePY() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.py", "/usercontent/u.py")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/u.py")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/u.py")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | python /usercontent/u.py"

	return out_loc, nil
}

func CompileJS() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.js", "/usercontent/u.js")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/u.js")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/u.js")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | node /usercontent/u.js"

	return out_loc, nil
}

func CompileTEST() (string, error) {

	var out_loc string

	cmd := exec.Command("cp", "/tmp/usercontent.test", "/usercontent/u.c")

	_, err := cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("cp", "/tmp/usercontent.input", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test cp: %s", err.Error())

	}

	cmd = exec.Command("gcc", "-o", "/usercontent/c", "/usercontent/u.c")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/c")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chown", "usercontent:usercontent", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chown: %s", err.Error())

	}

	cmd = exec.Command("chmod", "500", "/usercontent/c")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	cmd = exec.Command("chmod", "400", "/usercontent/input")

	_, err = cmd.Output()

	if err != nil {

		return out_loc, fmt.Errorf("compile test chmod: %s", err.Error())

	}

	out_loc = "cat /usercontent/input | /usercontent/c"

	return out_loc, nil
}
