run: compile
	@echo "------------------------------" && refgo main.rsl+LibraryEx.rsl+lexer.rsl -l20

test: compile
	@pytest -vvv tests/*.py

compile:
	@refc main.ref
	@refc lexer.ref
	@refc tests/lexer.ref

clean:
	@rm *.rsl *.lis


.PHONY: run test clean compile
