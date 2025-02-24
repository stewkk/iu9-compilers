package main

import (
	"fmt"
)

func main() {
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
