OUT_DIR=./final_experiments/epoch-tau-dist/
mkdir -p $OUT_DIR
rm run.log

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 256 -q 4096 -z 2 -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 512 -q 2048 -z 4 -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 100 -q 4096 -z 1 -M window -F $OUT_DIR > run.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 12 -p 256 -x 8192 -0 128 -F $OUT_DIR > run.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 12 -p 256 -x 8192 -0 128 -F $OUT_DIR > run.log
