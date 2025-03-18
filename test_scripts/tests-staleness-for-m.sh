OUT_DIR=./final_experiments/staleness_for_m/
mkdir -p $OUT_DIR

set -x # echo on

./cmake-build/mininn -n 256 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 128 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 64 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 32 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 16 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 8 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
./cmake-build/mininn -n 4 -e 10 -b 32 -D async -M window -F $OUT_DIR > run.log
