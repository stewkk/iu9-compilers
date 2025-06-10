test:
	@find tests/ -name '*.c' -exec echo {} \; -exec python main.py {} \;

test_semantics:
	@find tests/ -name 'semantic_*.c' -exec echo {} \; -exec python main.py {} \;

.PHONY: test
