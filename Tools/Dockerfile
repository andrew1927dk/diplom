FROM ubuntu:20.04
CMD ["/bin/bash"]
WORKDIR /image
COPY . .
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y -V \
        ca-certificates \
        software-properties-common \
        wget \
        git \
        bzip2 \
        make \
    && apt-get clean

RUN wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2" \
    | tar -xj #buildkit

RUN wget https://apt.llvm.org/llvm.sh \
    && chmod +x llvm.sh \
    && ./llvm.sh 16 \
    && apt-get update \
    && apt-get install -y -V clang-format \
    && apt-get clean
    

ENV PATH=/image/gcc-arm-none-eabi-10.3-2021.10/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
