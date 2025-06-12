test:
	@find tests/ -name '*.c' -print0 | sort -z | xargs --null -I {} sh -c 'echo "================================================================" && echo {} && python main.py {}'

test_semantics:
	@find tests/ -name 'semantic_*.c' -print0 | sort -z | xargs --null -I {} sh -c 'echo "================================================================" && echo {} && python main.py {}'

.PHONY: test
