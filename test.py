import subprocess
from os import mkdir

grads = [
    # 0.000000,
    0.000025,
    0.000050,
    # 0.000075,
    0.000100,
    0.000200,
    0.000400,
]

G_factors = [0, -10, -50, 10]
J_factors = [0, -5, -15]
M_factors = [0, 0.5, 1.0, 2]


try:
    mkdir("runtime_logs")
except FileExistsError:
    pass

NUM_REPS = 5

# for G in G_factors:
#     for J in J_factors:
#         for M in M_factors:
#             for t_i in range(NUM_REPS):
#                 with open(f"autotests_30apr/elasync-G{G}-J{J}-M{M}-#{t_i}.txt", "w") as ofile:
#                     cmd = f"./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -e 200 -b 16 -l 0.005 -w 16 -i 128 -d 32 -s 128 -G '{G}' -J '{J}' -M '{M}' -N auto_skewing_test_{G}_{J}_{M}_#{t_i}"
#                     print(f"{t_i} / {NUM_REPS-1}: {cmd}")
#                     subprocess.call(cmd.split(" "), stdout=ofile)

for gr in grads:
    for t_i in range(NUM_REPS):
        with open(f"runtime_logs/linear-{gr}-#{t_i}.txt", "w") as ofile:
            # Test CIFAR10
            cmd = f"./cmake-build/mininn -a HEURISTIC -G {gr} -A LENET -D CIFAR10 -n 100 -e 200 -b 16 -l 0.005 -N may-linear-start100-grad{gr}-#{t_i}"
            print(f"({t_i}) {cmd}")
            subprocess.call(cmd.split(" "), stdout=ofile)
        #
        # with open(f"runtime_logs/linear-cifar100-{gr}-#{t_i}.txt", "w") as ofile:
        #     # Test CIFAR100
        #     cmd = f"./cmake-build/mininn -a HEURISTIC -G {gr} -A LENET -D CIFAR100 -n 128 -e 300 -b 16 -l 0.005 -N linear-cifar100-{gr}-#{t_i}"
        #     print(f"({t_i}) {cmd}")
        #     subprocess.call(cmd.split(" "), stdout=ofile)

# for t_i in range(NUM_REPS):
#     with open(f"runtime/elasync-#{t_i}.txt", "w") as ofile:
#         # Test elasync CIFAR10
#         cmd = f"./cmake-build/mininn -a ELASYNC -A LENET -D CIFAR10 -n 512 -s 128 -e 200 -b 16 -l 0.005 -w 8 -i 512 -d 32 -N elasync-#{t_i}"
#         print(f"({t_i}) {cmd}")
#         subprocess.call(cmd.split(" "), stdout=ofile)
