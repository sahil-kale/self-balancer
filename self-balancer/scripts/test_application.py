import os
import argparse

def main(debug):
    #chdir to "application"
    os.chdir("application")

    # make a build directory
    os.system("mkdir -p build")
    os.chdir("build")

    # run "cmake ."
    cmd = r'cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"'
    os.system(cmd)
    # make the application, check that the ret code is 0
    ret = os.system("make all")
    if ret != 0:
        print("Error building the application")
        exit(1)

    if debug:
        os.system("gdb ./application_tests")
    else:
        os.system("./application_tests")

if __name__ == "__main__":
    # create an arg parser for whether to debug or not
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", help="Debug mode", action="store_true")
    args = parser.parse_args()

    main(args.debug)

