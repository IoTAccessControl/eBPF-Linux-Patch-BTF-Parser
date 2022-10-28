self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`


build_apache() {
	if [[ ! -d ${self_dir}/httpd-2.4.46 ]]; then
		echo "download httpd-2.4.46"
		wget https://dlcdn.apache.org//apr/apr-1.7.0.tar.gz -O - | tar xz
        wget https://dlcdn.apache.org//apr/apr-util-1.6.1.tar.gz -O - | tar xz
        wget https://dlcdn.apache.org/httpd/httpd-2.4.54.tar.gz -O - | tar xz
	fi
    # install apr
	cd apr-1.7.0
    ./configure --prefix=/usr/local/apr
    make && sudo make install
    #install apr-util
    cd ../apr-util-1.6.1
    ./configure --prefix=/usr/local/apr-util --with-apr=/usr/local/apr
    make && sudo make install
    #install httpd
    cd ../httpd-2.4.54
    ./configure --prefix=/usr/local/apache --with-apr=/usr/local/apr --with-apr-util=/usr/local/apr-util/
    make
}

pushd ${self_dir}
build_apache
popd