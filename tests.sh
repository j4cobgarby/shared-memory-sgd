OUT_FOLDER=reduce_period_every_4096_steps
mkdir -p $OUT_FOLDER

./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 100 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
