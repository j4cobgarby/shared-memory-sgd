OUT_DIR=./final_experiments/3_overnight_constant_semisync_vs_async_rep_2-and-more
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c decay -y 128 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -c decay -y 4 -z 0 -M window -F $OUT_DIR >> 3_run.log

./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -c decay -y 128 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -c decay -y 32 -z 0 -M window -F $OUT_DIR >> 3_run.log

./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -c decay -y 512 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -c decay -y 128 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log

./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -c decay -y 1024 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -c decay -y 256 -z 0 -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -c decay -y 64 -z 0 -M window -F $OUT_DIR >> 3_run.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
./cmake-build/mininn -n 16 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> 3_run.log
