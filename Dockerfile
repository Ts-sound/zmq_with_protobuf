# Base image.
FROM ubuntu:18.04

# Install dependencies.
RUN apt update && apt install -y \
    build-essential \
    git  git-core \
    cmake \
    libssl-dev \
    gcc gdb \
    openssh-server \
    libzmqpp-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# create sshd run dir
RUN mkdir -p /var/run/sshd

# set password
RUN echo 'root:1' | chpasswd 

# set ssh root permit
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config 
RUN sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

# expose 22
EXPOSE 22

# start sshd
CMD ["/usr/sbin/sshd","-D"]