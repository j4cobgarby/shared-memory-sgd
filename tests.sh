OUT_FOLDER=correct_async_window_vs_semisync_static
mkdir -p $OUT_FOLDER

./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 128 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 256 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 270 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 512 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 1024 -M eval -F $OUT_FOLDER

./cmake-build/mininn -n 128 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 64 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 128 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 164 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 256 -M eval -F $OUT_FOLDER
./cmake-build/mininn -n 128 -b 16 -l 0.005 -u 0.35 -e 300 -P static -D semisync -y 512 -M eval -F $OUT_FOLDER

./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P window -w 32 -p 1024 -x 20000 -D async -M eval -F $OUT_FOLDER -0 128
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P window -w 16 -p 1024 -x 20000 -D async -M eval -F $OUT_FOLDER -0 128

./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P window -w 32 -p 1024 -x 20000 -D async -M eval -F $OUT_FOLDER -0 256
./cmake-build/mininn -n 256 -b 16 -l 0.005 -u 0.35 -e 300 -P window -w 16 -p 1024 -x 20000 -D async -M eval -F $OUT_FOLDER -0 256
