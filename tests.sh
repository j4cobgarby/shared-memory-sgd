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
mkdir -p experiments/25phase_optimalprobes
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 2000 -b 16 -l 0.005 -i 256 -d 2048 -N optimal_auto10 | tee experiments/25phase_optimalprobes/run10.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 2000 -b 16 -l 0.005 -i 256 -d 2048 -N optimal_auto11 | tee experiments/25phase_optimalprobes/run11.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 2000 -b 16 -l 0.005 -i 256 -d 2048 -N optimal_auto12 | tee experiments/25phase_optimalprobes/run12.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 2000 -b 16 -l 0.005 -i 256 -d 2048 -N optimal_auto13 | tee experiments/25phase_optimalprobes/run13.csv
./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 2000 -b 16 -l 0.005 -i 256 -d 2048 -N optimal_auto14 | tee experiments/25phase_optimalprobes/run14.csv
