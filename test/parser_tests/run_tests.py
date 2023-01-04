#!/usr/bin/python3

import os
from pathlib import Path
import subprocess

EXE = "../../build/bin/jcc"

def list_all_tests():
    tests = []
    for _, _, files in os.walk("."):
        for file in files:
            if file.endswith(".c"):
                tests.append(file)
    return tests


def run_test(test_file):
    expected_file = Path(test_file).stem + ".out"
    actual = subprocess.run(
        [EXE, test_file, "--ast-dump"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    ).stdout
    with open(expected_file, "r") as f:
        expected = f.read()
        if expected == actual:
            print('\033[32m' + f"{test_file} ====> OK!" + '\033[m')
        else:
            print('\033[31m' + f"{test_file} ====> FAIL!" + '\033[m')
            exit(-1)


if __name__ == "__main__":
    for test_file in list_all_tests():
        run_test(test_file)
