test:
	@find tests/ -name '*.c' -exec echo {} \; -exec python main.py {} \;

test_semantics:
	@find tests/ -name 'semantic_*.c' -print0 | sort -z | xargs --null -I {} sh -c 'echo {} && python main.py {}'

.PHONY: test
