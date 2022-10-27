self_path=`readlink -f "${BASH_SOURCE:-$0}"`
self_dir=`dirname $self_path`
WORK_DIR="$self_dir/../deps"

cd ${WORK_DIR}/dwarves/lib/bpf/
git diff > pahole.patch
git format-patch pahole.patch
mv pahole.patch ${self_dir}/../patch/pahole.patch