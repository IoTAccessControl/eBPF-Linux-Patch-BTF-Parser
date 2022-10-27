self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`


build_openssl() {
	if [[ ! -d ${self_dir}/openssl-1.1.1q ]]; then
		echo "download openssl-1.1.1q"
		wget http://www.openssl.org/source/openssl-1.1.1q.tar.gz 
        tar zxvf openssl-1.1.1q.tar.gz
	fi
	cd openssl-1.1.1q
	./config -d --prefix=/usr/local/openssl
	make
    rm openssl-1.1.1q.tar.gz
}

pushd ${self_dir}
build_openssl
popd