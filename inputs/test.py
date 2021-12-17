import subprocess
import os
import re

#BINARY_PATH = "bin/mycc"
interpreter = "./bin/interpreter"
machine = "./bin/toAssembly"
runShell = "./run.sh"
files = [   
            ["operators.c", "0"],
            ["basic1.c", "0"],
            ["basic2.c", "5"],
            ["basic3.c", "4"],
            ["branch1.c", "100"],
            ["branch2.c", "-1"],
            ["count.c", "100"],
            ["factorial.c", "3628800"],
            ["functions1.c", "10"],
            ["functions2.c", "1"],
            ["functions3.c", "11"],
            ["functions4.c", "2"],
            ["functions5.c", "3"],
            ["functions6.c", "2"],
            ["HCF.c", "4"],
            ["loop1.c", "1"],
            ["loop3.c", "2001"],
            ["slp1.c", "0"],
            ["lexscope1.c", "126"],
            ["lexscope2.c", "11"],
            ["lexscope3.c", "5"],
            ["lexscope4.c", "6"],
            ["lexscope5.c", "3"]
        ]

class Tester:

    def test_return(self):
        code = "int main() {return 1;}"
        result = "1"
        self.test_helper(code, result)

    def testAll(self):
        

        for fileName in files:
            file = open("./inputs/" + fileName[0], mode='r')
            f = file.read()
            file.close()

            self.test_helper(fileName[0], f, fileName[1], "0")
            self.test_helper(fileName[0], f, fileName[1], "1")

    def test_helper(self, fileName: str, input_code: str, output_regex: str, mode: str):
        print(f"Testing {fileName}: ")
        process = subprocess.Popen(
            [runShell, mode, fileName], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, err = process.communicate(input=bytes(input_code, "utf-8'"))

        message = f"Failure messages in {fileName}\nStderr: \n{err.decode('utf-8')}\n"

        self.longMessage = False

        if(process.returncode != 0):
            print(message)

        stdout = output.decode("utf-8'")

        if re.search(f"Program exited with code: {output_regex}\n", stdout):
            print("Success")
        else:
            #print(message)
            print("Fail:")
            print(stdout[-64:])


        print("------------------")

if __name__ == '__main__':
    Tester().testAll()
