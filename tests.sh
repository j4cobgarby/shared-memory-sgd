OUT_FOLDER=semisync_runs_3
mkdir -p $OUT_FOLDER

# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 270 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 512 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 1024 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 4096 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 256 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D async -M window -F $OUT_FOLDER

./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 32 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 512 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 1024 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 4096 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D async -M window -F $OUT_FOLDER
#
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 270 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 512 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 1024 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 4096 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 16 -u 0.3 -l 0.005 -e 400 -P static -D async -M window -F $OUT_FOLDER

./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 32 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 512 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 1024 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D semisync -y 4096 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 8 -u 0.3 -l 0.005 -e 400 -P static -D async -M window -F $OUT_FOLDER
