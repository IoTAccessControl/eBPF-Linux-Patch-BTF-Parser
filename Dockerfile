FROM ubuntu:22.04
LABEL maintainer="Yi He, Shuo Li"
LABEL Description="Image for building and testing BTF parser"

ARG DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && \
	apt-get install --no-install-recommends -y git vim clang cmake build-essential \
		g++ git zlib1g-dev llvm python3 python3-pip iproute2 libelf-dev \
		libdw-dev libpcre3-dev curl

WORKDIR /work/eBPF-Linux-Patch-BTF-Parser
ADD . /work/eBPF-Linux-Patch-BTF-Parser 

RUN curl https://cloud.tsinghua.edu.cn/f/86f47c30ba464a2aae97/?dl=1 -o /bin/bpftool
RUN chmod +x /bin/bpftool

RUN bash scripts/build_all.sh
RUN bash test_prog/01-simple_test/1-gen_btf.sh
RUN bash test_prog/01-simple_test/2-parse_btf.sh 
RUN bash test_prog/02-openssl_test/1-gen_btf.sh
RUN bash test_prog/03-nginx_test/1-gen_btf.sh
RUN bash test_prog/03-nginx_test/2-parse_btf.sh
RUN bash test_prog/04-apache_test/1-gen_btf.sh
RUN bash test_prog/05-memcached_test/1-gen_btf.sh

CMD ["bash"]