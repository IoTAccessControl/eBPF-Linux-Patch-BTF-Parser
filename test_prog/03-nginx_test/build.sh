self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`


build_nginx() {
	if [[ ! -d ${self_dir}/nginx-1.22.1 ]]; then
		echo "download nginx-1.22.1"
		curl http://nginx.org/download/nginx-1.22.1.tar.gz | tar zxvf -
	fi
	cd nginx-1.22.1
	./configure
	make
}

pushd ${self_dir}
build_nginx
popd