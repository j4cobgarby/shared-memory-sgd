OUT_DIR=./final_experiments/5reps-semisync-adjust-period
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 100 -z 1 -q 4096 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 100 -z 1 -q 8192 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 200 -z 2 -q 4096 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 200 -z 2 -q 8192 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 1000 -z 2 -q 1638 -M window -F $OUT_DIR >> run_23j4.log

./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 60 -z 1 -q 8192 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 100 -z 1 -q 4096 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 100 -z 1 -q 8192 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 200 -z 2 -q 4096 -M window -F $OUT_DIR >> run_23j4.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 200 -z 2 -q 8192 -M window -F $OUT_DIR >> run_23j4.log
