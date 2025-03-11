OUT_FOLDER=fashionmnist/tests1
mkdir -p $OUT_FOLDER

./cmake-build/mininn -A FASHION-MNIST -n 8 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
./cmake-build/mininn -A FASHION-MNIST -n 16 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
./cmake-build/mininn -A FASHION-MNIST -n 24 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
./cmake-build/mininn -A FASHION-MNIST -n 48 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
./cmake-build/mininn -A FASHION-MNIST -n 64 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
./cmake-build/mininn -A FASHION-MNIST -n 128 -b 16 -u 0.5 -l 0.002 -P static -D async -F $OUT_FOLDER
