FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

RUN apt update
RUN apt upgrade -y
RUN apt install -y gawk

RUN awk '{sub(/deb/,"deb [arch=amd64]")}1' /etc/apt/sources.list > temp.txt && mv temp.txt /etc/apt/sources.list

RUN printf "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy main restricted universe multiverse\ndeb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-updates main restricted universe multiverse\ndeb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-backports main restricted universe multiverse\ndeb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports jammy-security main restricted universe multiverse\ndeb [arch=arm64] http://archive.canonical.com/ubuntu jammy partner\n" >> /etc/apt/sources.list.d/arm64-sources.list
RUN dpkg --add-architecture arm64

RUN apt update
RUN apt upgrade -y

RUN apt install -y gcc g++ make gcc-aarch64-linux-gnu g++-aarch64-linux-gnu binutils-aarch64-linux-gnu

RUN apt install -y libdevil-dev:arm64
RUN apt install -y libsndfile1-dev:arm64

RUN apt install -y opencl-headers
RUN apt install -y ocl-icd-opencl-dev:arm64
RUN apt install -y ocl-icd-libopencl1:arm64

RUN apt install -y libgstreamer1.0-dev:arm64 libgstreamer-plugins-base1.0-dev:arm64 libgstreamer-plugins-good1.0-dev:arm64 libgstreamer-plugins-bad1.0-dev:arm64
