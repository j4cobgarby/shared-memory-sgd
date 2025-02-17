OUT_DIR=./m128_different_batch_size
mkdir -p $OUT_DIR

set -x # echo on

# ./cmake-build/mininn -n 128 -b 2 -s 25000 -u 0.3 -D async -M window -F $OUT_DIR
./cmake-build/mininn -n 128 -b 4 -s 12500 -u 0.3 -D async -M window -F $OUT_DIR -l 0.001
./cmake-build/mininn -n 128 -b 8 -s 6250 -u 0.3 -D async -M window -F $OUT_DIR -l 0.001
./cmake-build/mininn -n 128 -b 16 -s 3125 -u 0.3 -D async -M window -F $OUT_DIR -l 0.001

# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c decay -y 128 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c decay -y 4 -z 0 -M window -F $OUT_DIR >> 3_run.log
#
# ./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -c decay -y 128 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -c decay -y 32 -z 0 -M window -F $OUT_DIR >> 3_run.log
#
# ./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -c decay -y 512 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -c decay -y 128 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log
#
# ./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -c decay -y 1024 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -c decay -y 256 -z 0 -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log
#
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 64 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 48 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 32 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
# ./cmake-build/mininn -n 16 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
