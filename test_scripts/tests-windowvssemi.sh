OUT_DIR=./final_experiments/find_best_window_params_2/
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 8 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 12 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 20 -p 256 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log
./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 8 -p 1024 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log

# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M eval -P window -w 12 -p 1024 -x 8192 -0 128 -F $OUT_DIR >> run_window_params.log

# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 8 -p 4096 -x 16384 -0 128 -F $OUT_DIR >> run_window_params.log
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -D async -M window -P window -w 12 -p 4096 -x 16384 -0 128 -F $OUT_DIR >> run_window_params.log
