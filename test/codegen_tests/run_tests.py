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
    steam = Path(test_file).stem
    expected = steam + ".out"
    bin = steam + ".bin"
    subprocess.run(
        [EXE, test_file, "-o", bin],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    actual = subprocess.run(
        [f"./{bin}"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )

    with open(expected, "r") as f:
        expected_ret = f.readline().strip()
        expected_out = f.read()
        if actual.returncode == int(expected_ret):
            if len(expected_out) != 0:
                if expected_out != actual.stdout:
                    print("\033[31m" + f"{test_file} ====> FAIL!" + "\033[m")
                    exit(-1)
            print("\033[32m" + f"{test_file} ====> OK!" + "\033[m")
        else:
            print("\033[31m" + f"{test_file} ====> FAIL!" + "\033[m")
            exit(-1)


def cleanup():
    for _, _, files in os.walk("."):
        for file in files:
            if file.endswith(".s") or file.endswith(".o") or file.endswith(".bin"):
                os.remove(file)
            if file == "a.out":
                os.remove(file)

if __name__ == "__main__":
    for test_file in list_all_tests():
        run_test(test_file)
    cleanup()
