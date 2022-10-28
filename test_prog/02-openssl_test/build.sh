self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`


build_openssl() {
	if [[ ! -d ${self_dir}/openssl-3.0.5 ]]; then
		echo "download openssl-3.0.5"
		wget http://www.openssl.org/source/openssl-3.0.5.tar.gz -O - | tar xz
	fi
	cd openssl-3.0.5
	./config -d --prefix=/usr/local/openssl
	make
}

pushd ${self_dir}
build_openssl
popd