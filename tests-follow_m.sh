OUT_DIR=./test_exps/follow_m
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c follow_m -P window -0 128 -w 12 -x 8192 -p 1024 -F $OUT_DIR

