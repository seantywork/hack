package main

import (
	"fmt"
	"os"
	"os/exec"
	"time"

	jambuild "0xgo/kube-codejam/jam/build"
	jamwatch "0xgo/kube-codejam/jam/watch"
)

func RunPayloadJam(flag string, log_run string, log_out *os.File, log_err *os.File) error {

	var cmd *exec.Cmd

	switch flag {

	case "c":

		out_loc, err := jambuild.CompileC()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("/bin/bash", "-c", out_loc)

	case "cc":

		out_loc, err := jambuild.CompileCC()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("/bin/bash", "-c", out_loc)

	case "go":

		out_loc, err := jambuild.CompileGO()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("/bin/bash", "-c", out_loc)

	case "java":

		out_loc, err := jambuild.CompileJAVA()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("/bin/bash", "-c", out_loc)

	case "py":

		out_loc, err := jambuild.CompilePY()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("/bin/bash", "-c", out_loc)

	case "js":

		out_loc, err := jambuild.CompileJS()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("/bin/bash", "-c", out_loc)

	case "test":

		out_loc, err := jambuild.CompileTEST()

		if err != nil {
			return fmt.Errorf("compile error: %s", err.Error())
		}

		cmd = exec.Command("runuser", "-l", "usercontent", "-c", out_loc)

	default:

		return fmt.Errorf("wrong option: %s", flag)

	}

	if err := ExecWatch(cmd, log_run, log_out, log_err); err != nil {
		return fmt.Errorf("execution error: %s", err.Error())
	}

	return nil

}

func ExecWatch(cmd *exec.Cmd, log_run string, log_out *os.File, log_err *os.File) error {

	SIGRUNV := make(chan string)
	SIGOUT := make(chan int)
	SIGERR := make(chan string)

	cmd.Stdout = log_out

	cmd.Stderr = log_err

	var begin time.Time

	var pid int = 0

	err := cmd.Start()

	if err != nil {

		return fmt.Errorf("watch start: %s", err.Error())
	}

	begin = time.Now()

	pid = cmd.Process.Pid

	go jamwatch.WatchRunInfo(log_run, begin, pid, SIGRUNV)

	go func() {

		err := cmd.Wait()

		if err != nil {
			jamwatch.STOP_WATCH = 1
			SIGERR <- fmt.Errorf("watch wait: %s", err.Error()).Error()
			return
		}

		SIGOUT <- cmd.ProcessState.ExitCode()

		return

	}()

	wait_sig_out := 0

	for wait_sig_out == 0 {
		select {
		case runv_msg := <-SIGRUNV:

			return fmt.Errorf("violation observed: %s", runv_msg)

		case err_num := <-SIGERR:

			jamwatch.SaveWatchResult(log_run, "runtime error", 0, 0)

			return fmt.Errorf("error observed: %s", string(err_num))

		case exit_code := <-SIGOUT:

			fmt.Println(exit_code)
			wait_sig_out = 1
			break

		}
	}

	return nil
}

func main() {

	FLAG := os.Args[1]

	run_log_location := ""
	out_log_location := ""
	err_log_location := ""
	prog_location := ""

	if FLAG == "_test" {

		run_log_location = "./_test/run"
		out_log_location = "./_test/out"
		err_log_location = "./_test/err"

	} else {

		run_log_location = "/home/run"
		out_log_location = "/home/out"
		err_log_location = "/home/err"
		prog_location = "/home/prog"

	}

	log_run := run_log_location
	log_out, _ := os.Create(out_log_location)
	log_err, _ := os.Create(err_log_location)
	os.WriteFile(prog_location, []byte("Running"), 0644)

	if err := RunPayloadJam(FLAG, log_run, log_out, log_err); err != nil {
		os.WriteFile(prog_location, []byte("Error"), 0644)
		fmt.Printf("error: %s\n", err.Error())
	} else {
		os.WriteFile(prog_location, []byte("Completed"), 0644)
		fmt.Println("success")
	}

}
