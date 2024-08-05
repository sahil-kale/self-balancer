import os

# update apt
os.system("sudo apt-get update")
# upgrade apt
os.system("sudo apt-get upgrade -y")

# components to install via apt
apt_components = [
    "protobuf-compiler",
    "git",
    "clang-format",
    "clang-tidy",
    "build-essential",
    "gdb",
    "doxygen",
    "cmake"
]

# components to install via pip
pip_components = [
    "protobuf",
    "grpcio-tools"
]

# install apt components
for component in apt_components:
    os.system(f"sudo apt-get install -y {component}")

# install pip components
for component in pip_components:
    os.system(f"pip install {component}")

# update submodules
os.system("git submodule update --init --recursive")