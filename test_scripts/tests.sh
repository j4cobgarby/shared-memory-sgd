OUT_DIR=./final_experiments/accuracy_prelim
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -s 20000 -u 0.5 -D semisync -c probe -L 0.4 -S 4 -W 16 -y 100 -l 0.005 -F $OUT_DIR
./cmake-build/mininn -n 128 -b 16 -s 20000 -u 0.5 -D semisync -c probe -L 0.4 -S 4 -W 16 -y 200 -l 0.005 -F $OUT_DIR
./cmake-build/mininn -n 128 -b 16 -s 20000 -u 0.5 -D semisync -c probe -L 0.4 -S 4 -W 16 -y 400 -l 0.005 -F $OUT_DIR

# ./cmake-build/mininn -n 128 -b 16 -s 15000 -u 0.5 -D async -P window -p 1024 -x 8192 -w 12 -0 128 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 15000 -u 0.5 -D async -P window -p 1024 -x 8192 -w 12 -0 128 -l 0.002 -F $OUT_DIR
#
# ./cmake-build/mininn -n 128 -b 16 -s 25000 -u 0.5 -D async -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 25000 -u 0.5 -D semisync -y 2048 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 25000 -u 0.5 -D semisync -y 1024 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 25000 -u 0.5 -D semisync -y 512 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 20000 -u 0.5 -D semisync -y 256 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 10000 -u 0.5 -D semisync -y 128 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s 10000 -u 0.5 -D semisync -y 64 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 16 -s  6250 -u 0.5 -D semisync -y 32 -l 0.005 -F $OUT_DIR
#
# ./cmake-build/mininn -n 64 -b 16 -s 25000 -u 0.5 -D async -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s 25000 -u 0.5 -D semisync -y 2048 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s 25000 -u 0.5 -D semisync -y 1024 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s 25000 -u 0.5 -D semisync -y 512 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s 20000 -u 0.5 -D semisync -y 256 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s 10000 -u 0.5 -D semisync -y 128 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s 10000 -u 0.5 -D semisync -y 64 -l 0.005 -F $OUT_DIR
# ./cmake-build/mininn -n 64 -b 16 -s  6250 -u 0.5 -D semisync -y 32 -l 0.005 -F $OUT_DIR

# ./cmake-build/mininn -n 128 -b 2 -s 25000 -u 0.3 -D async -M window -F $OUT_DIR
# ./cmake-build/mininn -n 128 -b 4 -s 12500 -u 0.3 -D async -M window -F $OUT_DIR -l 0.001
# ./cmake-build/mininn -n 128 -b 8 -s 6250 -u 0.3 -D async -M window -F $OUT_DIR -l 0.001
# ./cmake-build/mininn -n 128 -b 16 -s 3125 -u 0.3 -D async -M window -F $OUT_DIR -l 0.001

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
