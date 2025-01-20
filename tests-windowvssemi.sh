
OUT_DIR=./final_experiments/find_best_window_params/
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 8 -p 1024 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 12 -p 1024 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 16 -p 1024 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 32 -p 1024 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 8 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 12 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 16 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 32 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log

# To do after this experiment: see if these are improved with semisync and the y=m/2 heuristic


# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 256 -q 4096 -z 2 -M window -F $OUT_DIR > run1.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 256 -q 4096 -z 4 -M window -F $OUT_DIR > run2.log
#
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 512 -q 2048 -z 2 -M window -F $OUT_DIR > run3.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 512 -q 2048 -z 4 -M window -F $OUT_DIR > run4.log
#
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 128 -q 4096 -z 1 -M window -F $OUT_DIR > run5.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 128 -q 4096 -z 2 -M window -F $OUT_DIR > run6.log
#
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D semisync -y 100 -q 4096 -z 1 -M window -F $OUT_DIR > run7.log
#
# echo "Async tests"
#
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run8.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run9.log
# ./cmake-build/mininn -n 64 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run10.log
# ./cmake-build/mininn -n 32 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run11.log
# ./cmake-build/mininn -n 16 -b 16 -u 0.3 -D async -M window -F $OUT_DIR > run12.log
