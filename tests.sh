OUT_FOLDER=semisync_weak_cas

PROBE_DUR=4000
EXEC_DUR=16000

mkdir -p $OUT_FOLDER

./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 50 -F $OUT_FOLDER -M eval -D semisync
./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 100 -F $OUT_FOLDER -M eval -D semisync
./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 200 -F $OUT_FOLDER -M eval -D semisync
./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 400 -F $OUT_FOLDER -M eval -D semisync
./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 800 -F $OUT_FOLDER -M eval -D semisync
