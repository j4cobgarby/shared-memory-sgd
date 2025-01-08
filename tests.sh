OUT_FOLDER=semisync_grid_test
mkdir -p $OUT_FOLDER

./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 8 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 16 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 32 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D async -M window -F $OUT_FOLDER

./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 8 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 16 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 32 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D async -M window -F $OUT_FOLDER

./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 8 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 16 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 32 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.0025 -e 400 -P static -D async -M window -F $OUT_FOLDER

./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 8 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 16 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 32 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.0025 -e 400 -P static -D async -M window -F $OUT_FOLDER
