package nextpermutation

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func nextPermutation(nums []int) {
	nums_len := len(nums)
	var peak_idx int = nums_len - 1
	var large_idx int = nums_len - 1

	for ; peak_idx > 0; peak_idx-- {
		if nums[peak_idx-1] < nums[peak_idx] {
			break
		}
	}

	if peak_idx != 0 {
		for ; large_idx >= peak_idx; large_idx-- {
			if nums[peak_idx-1] < nums[large_idx] {

				nums[large_idx], nums[peak_idx-1] = nums[peak_idx-1], nums[large_idx]
				break
			}
		}
	}

	for i := nums_len - 1; i > peak_idx; i, peak_idx = i-1, peak_idx+1 {
		nums[i], nums[peak_idx] = nums[peak_idx], nums[i]
	}
}

var NextPermutation func(nums []int) = nextPermutation

func NextPermutationInterface() {

	scanner := bufio.NewScanner(os.Stdin)

	scanner.Scan()

	var T int

	fmt.Sscanf(scanner.Text(), "%d", &T)

	for i := 0; i < T; i++ {

		var argLen int

		var nums []int

		scanner.Scan()

		argLenTxt := scanner.Text()

		fmt.Sscanf(argLenTxt, "%d", &argLen)

		scanner.Scan()

		args := scanner.Text()

		nums_str := strings.Split(args, " ")

		for j := 0; j < argLen; j++ {

			var el int

			fmt.Sscanf(nums_str[j], "%d", &el)

			nums = append(nums, el)

		}

		nextPermutation(nums)

		fmt.Println(nums)

	}

}
