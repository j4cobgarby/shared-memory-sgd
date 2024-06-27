# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a ASYNC -A LENET -N "async" -l 0.005
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi2048" -l 0.005 -i 2048
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi1024" -l 0.005 -i 1024
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi512" -l 0.005 -i 512
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi256" -l 0.005 -i 256
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi128" -l 0.005 -i 128
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi64" -l 0.005 -i 64
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi32" -l 0.005 -i 32
# ./cmake-build/mininn -D CIFAR10 -b 16 -e 350 -n 32 -a SEMISYNC -A LENET -N "semi16" -l 0.005 -i 16
#
mkdir -p experiments/optimal_auto
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 4000 -b 16 -l 0.005 -i 256 -d 1024 -N optimal_auto_0 | tee experiments/optimal_auto/run0.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 4000 -b 16 -l 0.005 -i 256 -d 1024 -N optimal_auto_1 | tee experiments/optimal_auto/run1.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 4000 -b 16 -l 0.005 -i 256 -d 1024 -N optimal_auto_2 | tee experiments/optimal_auto/run2.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 4000 -b 16 -l 0.005 -i 256 -d 1024 -N optimal_auto_3 | tee experiments/optimal_auto/run3.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 4000 -b 16 -l 0.005 -i 256 -d 1024 -N optimal_auto_4 | tee experiments/optimal_auto/run4.csv
