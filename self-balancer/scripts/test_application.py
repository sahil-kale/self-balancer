import os

#chdir to "application"
os.chdir("application")

# make a build directory
os.system("mkdir -p build")
os.chdir("build")

# run "cmake ."
cmd = r'cmake .. -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles"'
os.system(cmd)
os.system("make all")
os.system("./application_tests")