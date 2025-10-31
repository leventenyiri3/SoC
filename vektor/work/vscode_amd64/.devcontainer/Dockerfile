FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

RUN apt update
RUN apt upgrade -y
RUN apt install -y gawk

RUN apt update
RUN apt upgrade -y

RUN apt install -y gcc g++ make gcc g++ binutils

RUN apt install -y libdevil-dev
RUN apt install -y libsndfile1-dev

RUN apt install -y opencl-headers
RUN apt install -y ocl-icd-opencl-dev
RUN apt install -y ocl-icd-libopencl1

RUN apt install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-good1.0-dev libgstreamer-plugins-bad1.0-dev
