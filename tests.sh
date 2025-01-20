OUT_DIR=./final_experiments/semisync_comp/
mkdir -p $OUT_DIR

set -x # echo on

echo "Semi-sync tests"

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 256 -q 4096 -z 2 -M window -F $OUT_DIR > run1.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 256 -q 4096 -z 4 -M window -F $OUT_DIR > run2.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 512 -q 2048 -z 2 -M window -F $OUT_DIR > run3.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 512 -q 2048 -z 4 -M window -F $OUT_DIR > run4.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 128 -q 4096 -z 1 -M window -F $OUT_DIR > run5.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 128 -q 4096 -z 2 -M window -F $OUT_DIR > run6.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 100 -q 4096 -z 1 -M window -F $OUT_DIR > run7.log

echo "Async tests"

./cmake-build/mininn -n 256 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run8.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run9.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run10.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run11.log
./cmake-build/mininn -n 16 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run12.log
