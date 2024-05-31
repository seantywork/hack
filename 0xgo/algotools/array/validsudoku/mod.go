package validsudoku

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func isValidSudoku(board [][]byte) bool {

	var result bool

	return result
}

var IsValidSudoku func(board [][]byte) bool = isValidSudoku

func IsValidSudokuInterface() {

	scanner := bufio.NewScanner(os.Stdin)

	scanner.Scan()

	var T int

	fmt.Sscanf(scanner.Text(), "%d", &T)

	for i := 0; i < T; i++ {

		sudoku_board := make([][]byte, 9)

		for i := 0; i < 9; i++ {

			scanner.Scan()

			sudoku_line := scanner.Text()

			sudoku_split := strings.Split(sudoku_line, " ")

			for j := 0; j < 9; j++ {

			}

		}

	}

}
