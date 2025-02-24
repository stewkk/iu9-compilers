package main

import (
	"fmt"
)

func main() {
	for i := 0; i < 5; i++ {
		fmt.Println("cycle with init and post")
	}

	for fmt.Println("init"); ; fmt.Println("next") {
		break
	}

	i := 0
	for fmt.Println("init"); i < 5; fmt.Println("next") {
		fmt.Println(i)
		i += 1
		j := 2
		for fmt.Println("init"); j >= 0; fmt.Println("next") {
			fmt.Println("    ", j)
			j -= 1
		}
	}
}
