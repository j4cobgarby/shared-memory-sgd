OUT_FOLDER=eval_monitor_tests

PROBE_DUR=4000
EXEC_DUR=16000

mkdir -p $OUT_FOLDER

# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 8 -d 2 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 16 -d 2 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 32 -d 2 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 64 -d 2 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
#
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 32 -d 3 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 64 -d 3 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
#
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P ternary -w 128 -d 4 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
#
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P window -w 4 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P window -w 8 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P window -w 16 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P window -w 32 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
# ./cmake-build/mininn -n 500 -l 0.005 -u 0.35 -e 500 -P window -w 64 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER
#
# ./cmake-build/mininn -n 50 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
# ./cmake-build/mininn -n 100 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
# ./cmake-build/mininn -n 200 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
# ./cmake-build/mininn -n 400 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER

# Testing different batch sizes now

# ./cmake-build/mininn -n 200 -b 8 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
# ./cmake-build/mininn -n 200 -b 32 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
# ./cmake-build/mininn -n 200 -b 64 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
# ./cmake-build/mininn -n 200 -b 128 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
#
# ./cmake-build/mininn -n 500 -b 32 -l 0.005 -u 0.35 -e 500 -P ternary -w 64 -d 2 -p $PROBE_DUR -x $EXEC_DUR -F $OUT_FOLDER

./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 500 -P static -F $OUT_FOLDER
