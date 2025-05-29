#!/usr/bin/env python3

import pytest
import subprocess


@pytest.mark.parametrize("stdin, expected_stdout",
                         [
                             ("{ }",
                              ""),
                         ])
def test_lexer(stdin, expected_stdout):
    process = subprocess.run(
        [
            "refgo",
            "main.rsl+LibraryEx.rsl",
        ],
        capture_output=True,
        text=True,
        input=stdin,
    )
    output = process.stdout, process.stderr

    assert output == (expected_stdout, "")
