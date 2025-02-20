OUT_DIR=./accuracy_tests
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 64 -b 8 -s 12500 -u 0.5 -D semisync -c probe -M window -y 100 -W 16 -S 4 -L 0.4 -F $OUT_DIR -l 0.001
