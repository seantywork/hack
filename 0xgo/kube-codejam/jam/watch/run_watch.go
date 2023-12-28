package watch

import (
	"bufio"
	"bytes"
	"fmt"
	"os"
	"strconv"
	"time"
)

var STOP_WATCH = 0

var POSTED_USED_KB uint64 = 0

func WatchRunInfo(log_run string, begin time.Time, pid int, SIGRUNV chan string) {

	for STOP_WATCH == 0 {

		var runv_msg string = "SUCCESS"

		elapsed_ms := CalculateTime(begin)

		used_kb, err := CalculateAndPostMemory(pid)

		if used_kb > POSTED_USED_KB {

			POSTED_USED_KB = used_kb
		}

		if err != nil {
			fmt.Printf("WARNING: %s\n", err.Error())

		} else if elapsed_ms > 5000 {

			runv_msg = "TIME LIMIT EXCEEDED"
			SaveWatchResult(log_run, runv_msg, elapsed_ms, POSTED_USED_KB)
			SIGRUNV <- runv_msg
			break
		} else if POSTED_USED_KB > 50000 {

			runv_msg = "MEMORY LIMIT EXCEEDED"
			SaveWatchResult(log_run, runv_msg, elapsed_ms, POSTED_USED_KB)
			SIGRUNV <- runv_msg
			break
		} else {

			SaveWatchResult(log_run, runv_msg, elapsed_ms, POSTED_USED_KB)
		}

	}

	return

}

func CalculateTime(begin time.Time) int64 {

	duration := time.Since(begin)

	return duration.Milliseconds()

}

func CalculateAndPostMemory(pid int) (uint64, error) {

	f, err := os.Open(fmt.Sprintf("/proc/%d/smaps", pid))
	if err != nil {
		return 0, err
	}
	defer f.Close()

	res := uint64(0)
	pfx := []byte("Pss:")
	r := bufio.NewScanner(f)
	for r.Scan() {
		line := r.Bytes()
		if bytes.HasPrefix(line, pfx) {
			var size uint64
			_, err := fmt.Sscanf(string(line[4:]), "%d", &size)
			if err != nil {
				return 0, err
			}
			res += size
		}
	}
	if err := r.Err(); err != nil {
		return 0, err
	}

	return res, nil
}

func SaveWatchResult(log_run string, runv_msg string, elapsed_ms int64, used_kb uint64) {

	f, _ := os.OpenFile(log_run, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0644)

	elapsed_ms_str := strconv.FormatInt(elapsed_ms, 10)

	used_kb_str := strconv.FormatInt(int64(used_kb), 10)

	message := "RUNTIME_STATUS: " + runv_msg + "\n"
	message += "ELAPSED_TIME: " + elapsed_ms_str + "\n"
	message += "MEMORY_USAGE: " + used_kb_str

	f.Write([]byte(message))

}
