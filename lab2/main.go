package main

import (
	"fmt"
)

func main() {
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
