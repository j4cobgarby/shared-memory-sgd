OUT_FOLDER=./cifar100_with_accuracy/tests_elasync
mkdir -p $OUT_FOLDER

./cmake-build/mininn -A CIFAR100 -n 128 -s 8000 -b 16 -u 0.5 -l 0.005 -P window -0 128 -x 8192 -p 1024 -D async -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 96 -s 8000 -b 16 -u 0.5 -l 0.005 -P window -0 96 -x 8192 -p 1024 -D async -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 64 -s 8000 -b 16 -u 0.5 -l 0.005 -P window -0 64 -x 8192 -p 1024 -D async -F $OUT_FOLDER
./cmake-build/mininn -A CIFAR100 -n 32 -s 8000 -b 16 -u 0.5 -l 0.005 -P window -0 32 -x 8192 -p 1024 -D async -F $OUT_FOLDER
