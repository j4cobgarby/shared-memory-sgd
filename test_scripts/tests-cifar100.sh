OUT_FOLDER=cifar100_with_accuracy/tests_4
mkdir -p $OUT_FOLDER

./cmake-build/mininn -A CIFAR100 -n 128 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 96 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 64 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 32 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 64 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER

./cmake-build/mininn -A CIFAR100 -n 128 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 96 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 64 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 32 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 128 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER

./cmake-build/mininn -A CIFAR100 -n 128 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 96 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 64 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 32 -s 8000 -b 16 -u 0.5 -l 0.005 -P static -D semisync -y 256 -c probe -L 0.0 -W 16 -S 2 -m 4 -F $OUT_FOLDER
