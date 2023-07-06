FROM ubuntu:22.04

LABEL maintainer="Xijia Che"
LABEL Description="Image for building and testing CVE-2016-8704"

RUN apt-get update && apt-get install -y --no-install-recommends \
    libevent-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src

# install dependencies
RUN buildDeps=' \
    gcc \
    libc6-dev \
    libevent-dev \
    make \
    perl \
    wget \
    vim \
    ' \
    && set -x \
    && apt-get update && apt-get install -y $buildDeps --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

# install memcached-1.4.32 for testing
RUN wget --no-check-certificate -O memcached-1.4.32.tar.gz https://cloud.tsinghua.edu.cn/f/f5b6726657314ce4a643/?dl=1\
    && tar -cvzf memcached-1.4.32.tar.gz memcached-1.4.32\
    && cd memcached-1.4.32\
    && ./configure --prefix=/usr/local/memcached\
    && make CFLAGS="-g -O0"\ 
    && make install 

# download poc crash script
RUN wget --no-check-certificate -O exploit.py https://cloud.tsinghua.edu.cn/f/4ea87c8ee3a5407d9099/?dl=1

EXPOSE 11211
CMD ["bash"]