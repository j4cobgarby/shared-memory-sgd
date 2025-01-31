OUT_DIR=./final_experiments/2_overnight_constant_semisync_vs_async_rep_2-and-more
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 256 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 128 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 64 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 32 -z 0 -M window -F $OUT_DIR >> run_23j3.log

./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 128 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 64 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 32 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D semisync -y 16 -z 0 -M window -F $OUT_DIR >> run_23j3.log

./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -y 128 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -y 64 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -y 32 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D semisync -y 16 -z 0 -M window -F $OUT_DIR >> run_23j3.log

./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -y 64 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -y 32 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -y 16 -z 0 -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D semisync -y 8 -z 0 -M window -F $OUT_DIR >> run_23j3.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 64 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 48 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> run_23j3.log
./cmake-build/mininn -n 32 -b 16 -u 0.3 -D async -M window -F $OUT_DIR >> run_23j3.log

