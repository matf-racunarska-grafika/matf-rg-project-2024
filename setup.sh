sudo add-apt-repository universe
sudo apt update
sudo apt install g++-13 clang-format clang-tidy cmake git build-essential libwayland-dev libxkbcommon-dev xorg-dev libgl1-mesa-dev mesa-common-dev mesa-utils doxygen

glxinfo | grep OpenGL