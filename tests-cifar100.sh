OUT_FOLDER=cifar100_with_accuracy/lenet5
mkdir -p $OUT_FOLDER


./cmake-build/mininn -A CIFAR100 -n 32 -s 16384 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 16 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 32 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 48 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 64 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 96 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P window -p 4096 -x 16384 -w 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P window -p 4096 -x 16384 -w 4 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 512 -c probe -W 32 -S 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 256 -c probe -W 32 -S 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 128 -c probe -W 16 -S 4 -F $OUT_FOLDER
#
# ./cmake-build/mininn -A CIFAR100 -n 8 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 16 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 32 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 48 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 64 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 96 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P window -p 4096 -x 16384 -w 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P window -p 4096 -x 16384 -w 4 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 512 -c probe -W 32 -S 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 256 -c probe -W 32 -S 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 128 -c probe -W 16 -S 4 -F $OUT_FOLDER
#
# ./cmake-build/mininn -A CIFAR100 -n 8 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 16 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 32 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 48 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 64 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 96 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D async -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P window -p 4096 -x 16384 -w 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P window -p 4096 -x 16384 -w 4 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 512 -c probe -W 32 -S 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 256 -c probe -W 32 -S 8 -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -s 16384 -b 8 -u 0.5 -l 0.0025 -P static -D semisync -y 128 -c probe -W 16 -S 4 -F $OUT_FOLDER
