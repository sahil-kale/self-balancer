import os

# components to install via apt
apt_components = [
    "protobuf-compiler"
]

# components to install via pip
pip_components = [
    "protobuf"
    "grpcio-tools"
]

# install apt components
for component in apt_components:
    os.system(f"sudo apt-get install -y {component}")

# install pip components
for component in pip_components:
    os.system(f"pip install {component}")
