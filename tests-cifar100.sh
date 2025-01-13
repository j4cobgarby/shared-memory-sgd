OUT_FOLDER=cifar100_new_arch
mkdir -p $OUT_FOLDER

# ./cmake-build/mininn -A CIFAR100 -n 128 -b 2 -u 0.3 -l 0.0025 -e 600 -P static -D async -M window -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 128 -b 4 -u 0.3 -l 0.0025 -e 600 -P static -D async -M window -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 128 -b 8 -u 0.3 -l 0.0025 -e 600 -P static -D async -M window -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 128 -b 16 -u 0.3 -l 0.0025 -e 600 -P static -D async -M window -F $OUT_FOLDER
#
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 2 -u 0.3 -l 0.005 -e 600 -P static -D async -M window -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 4 -u 0.3 -l 0.005 -e 600 -P static -D async -M window -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 8 -u 0.3 -l 0.005 -e 600 -P static -D async -M window -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 128 -b 16 -u 0.3 -l 0.005 -e 600 -P static -D async -M window -F $OUT_FOLDER
#
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 2 -u 0.3 -l 0.01 -e 600 -P static -D async -M window -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 4 -u 0.3 -l 0.01 -e 600 -P static -D async -M window -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 8 -u 0.3 -l 0.01 -e 600 -P static -D async -M window -F $OUT_FOLDER
# ./cmake-build/mininn -A CIFAR100 -n 128 -b 16 -u 0.3 -l 0.01 -e 600 -P static -D async -M window -F $OUT_FOLDER
