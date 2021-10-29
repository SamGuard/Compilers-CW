import subprocess
import os
import re

#BINARY_PATH = "bin/mycc"
COMMAND = "./main.exe"


class TestInterpreter:

    def test_interpreter_return(self):
        code = "int main() {return 1;}"
        result = "1"
        self.interpreter_test_helper(code, result)

    def testAll(self):
        files = [
            ["basic1.c", "0"],
            ["basic2.c", "4"],
            ["branch1.c", "100"],
            ["count.c", "1000000"],
            ["factorial.c", "3628800"],
            ["HCF.c", "4"],
            ["loop1.c", "1"],
            ["loop2.c", "20"],
            ["loop3.c", "2001"],
            ["slp1.c", "-15"]
        ]

        for fileName in files:
            file = open("./inputs/" + fileName[0], mode='r')
            f = file.read()
            file.close()

            self.interpreter_test_helper(fileName[0], f, fileName[1])

    def interpreter_test_helper(self, fileName: str, input_code: str, output_regex: str):
        print(f"Testing {fileName}: ")
        process = subprocess.Popen(
            [COMMAND], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, err = process.communicate(input=bytes(input_code, "ascii"))

        message = f"Failure messages in {fileName}\nStderr: \n{err.decode('ascii')}\nStdout: \n{output.decode('ascii')}"

        self.longMessage = True

        if(process.returncode != 0):
            print(message)

        stdout = output.decode("ascii")

        if re.search(f"Program exited with code: {output_regex}\n", stdout):
            print("Success")
        else:
            print(message)

        print("------------------")

if __name__ == '__main__':
    TestInterpreter().testAll()
