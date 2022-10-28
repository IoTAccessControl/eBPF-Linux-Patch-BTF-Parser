self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`


build_memcached() {
	if [[ ! -d ${self_dir}/memcached-1.6.17 ]]; then
		echo "download memcached-1.6.17"
        wget https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz -O - | tar xz
        wget https://memcached.org/files/memcached-1.6.17.tar.gz -O - | tar xz
	fi
    # install libevent
    cd libevent-2.1.12-stable
    ./configure --prefix=/usr/local/libevent
    make && sudo make install
    # install memcached
    cd ../memcached-1.6.17
    ./configure --prefix=/usr/local/memcached --with-libevent=/usr/local/libevent
    make CFLAGS="-g -O0"
}

pushd ${self_dir}
build_memcached
popd