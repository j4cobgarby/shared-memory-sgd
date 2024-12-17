OUT_FOLDER=lockfree_semisync_fixed3
mkdir -p $OUT_FOLDER

./cmake-build/mininn -n 256 -b 16 -l 0.005 -e 200 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -e 200 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -e 200 -P static -D semisync -y 270 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -e 200 -P static -D semisync -y 512 -M window -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -e 200 -P static -D semisync -y 1024 -M window -F $OUT_FOLDER

# ./cmake-build/mininn -n 128 -b 32 -l 0.005 -e 200 -P static -D semisync -y 64 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 32 -l 0.005 -e 200 -P static -D semisync -y 128 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 32 -l 0.005 -e 200 -P static -D semisync -y 164 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 32 -l 0.005 -e 200 -P static -D semisync -y 256 -M window -F $OUT_FOLDER
# ./cmake-build/mininn -n 128 -b 32 -l 0.005 -e 200 -P static -D semisync -y 512 -M window -F $OUT_FOLDER
