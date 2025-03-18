OUT_FOLDER=cifar10_with_accuracy/tests_yprobe
mkdir -p $OUT_FOLDER

./cmake-build/mininn -A CIFAR10 -n 128 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 96 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 64 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 32 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER

./cmake-build/mininn -A CIFAR10 -n 128 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 96 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 64 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 32 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER

./cmake-build/mininn -A CIFAR10 -n 128 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 96 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 64 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR10 -n 32 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
