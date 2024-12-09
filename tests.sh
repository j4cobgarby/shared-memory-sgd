OUT_FOLDER=semisync_windowprobe

mkdir -p $OUT_FOLDER

# ./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 50 -F $OUT_FOLDER -M eval -D semisync
# ./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 100 -F $OUT_FOLDER -M eval -D semisync
# ./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 200 -F $OUT_FOLDER -M eval -D semisync
# ./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 400 -F $OUT_FOLDER -M eval -D semisync
# ./cmake-build/mininn -n 200 -b 16 -l 0.005 -u 0.35 -e 150 -P static -y 800 -F $OUT_FOLDER -M eval -D semisync

./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 200 -P window -w 10 -p 1000 -x 15000 -F $OUT_FOLDER -M eval -D semisync -y 100
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 200 -P window -w 10 -p 1000 -x 15000 -F $OUT_FOLDER -M eval -D async
