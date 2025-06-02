run: compile
	@echo "------------------------------" && refgo main.rsl+LibraryEx.rsl+lexer.rsl

test: compile
	@pytest -vvv tests/*.py

compile:
	@refc main.ref
	@refc lexer.ref

clean:
	@rm *.rsl *.lis


.PHONY: run test clean compile
