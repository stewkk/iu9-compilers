package main

import (
	"fmt"
)

func main() {
	for i := 0; i < 5; i++ {
		fmt.Println("cycle with init and post")
	}

	for {
		break
	}

	i := 0
	for i < 5 {
		fmt.Println(i)
		i += 1
		j := 2
		for j >= 0 {
			fmt.Println("    ", j)
			j -= 1
		}
	}
}
