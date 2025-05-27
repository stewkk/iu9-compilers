run: compile
	@echo "------------------------------" && refgo main.rsl+LibraryEx.rsl

test: compile
	@pytest -vvv tests/*.py

compile:
	@refc main.ref

clean:
	@rm *.rsl *.lis


.PHONY: run test clean compile
