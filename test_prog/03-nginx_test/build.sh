self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`


build_nginx() {
	if [[ ! -d ${self_dir}/nginx-1.22.1 ]]; then
		echo "download nginx-1.22.1"
		curl http://nginx.org/download/nginx-1.22.1.tar.gz | tar zxvf -
	fi
	cd nginx-1.22.1
<<<<<<< HEAD
	./configure # --with-http_ssl_module
	make -j4
=======
	./configure --prefix=/usr/local/nginx --with-http_ssl_module --with-ld-opt="-L../../02-openssl_test/openssl-3.0.5"
	make CFLAGS="-g -O0"
>>>>>>> 750c5521ff7309e9434c1e2cbade66c65bf86e55
}

pushd ${self_dir}
build_nginx
popd
