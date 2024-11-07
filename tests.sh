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
mkdir -p experiments/static_tests
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 5 -e 800 -b 16 -l 0.005 -s 5 -d 4096 -N static_tests | tee experiments/static_tests/run0.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 25 -e 800 -b 16 -l 0.005 -s 25 -d 4096 -N static_tests | tee experiments/static_tests/run1.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 50 -e 800 -b 16 -l 0.005 -s 50 -d 4096 -N static_tests | tee experiments/static_tests/run2.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 100 -e 800 -b 16 -l 0.005 -s 100 -d 4096 -N static_tests | tee experiments/static_tests/run3.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 300 -e 800 -b 16 -l 0.005 -s 300 -d 4096 -N static_tests | tee experiments/static_tests/run4.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 500 -e 800 -b 16 -l 0.005 -s 500 -d 4096 -N static_tests | tee experiments/static_tests/run4.csv
