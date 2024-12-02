import subprocess
from os import mkdir

batch_sizes = [16]
static_paras = [20, 100, 200, 400]

log_dir = 'paramtest_logs'

try:
    mkdir(log_dir)
except FileExistsError:
    pass

NUM_REPS = 3

for bs in batch_sizes:
    for mu in [0.4]:
        for ws in [8, 16, 32, 64]:
            for pd, xd in [(1000, 8000), (4000, 16000)]:
                for ti in range(NUM_REPS):
                    with open(f"{log_dir}/window-{bs}-{mu}-{ws}-{pd}-{xd}-#{ti}.txt", "w") as ofile:
                        cmd = f"./cmake-build/mininn -n 512 -l 0.005 -u {mu} -b {bs} -e 500 -P window -M eval -p {pd} -x {xd} -w {ws} -F paramtest_jsons"
                        print(f"Running '{cmd}' rep {ti}")
                        subprocess.call(cmd.split(" "), stdout=ofile)
                    for sd in [2,3]:
                        with open(f"{log_dir}/ternary-{bs}-{mu}-{ws}-{pd}-{xd}-{sd}-#{ti}.txt", "w") as ofile:
                            cmd = f"./cmake-build/mininn -n 512 -l 0.005 -u {mu} -b {bs} -e 500 -P ternary -M eval -p {pd} -x {xd} -w {ws} -F paramtest_jsons"
                            print(f"Running '{cmd}' rep {ti}")
                            subprocess.call(cmd.split(" "), stdout=ofile)
        for ti in range(NUM_REPS):
            for m in static_paras:
                with open(f"{log_dir}/static-{bs}-{mu}-{m}-#{ti}.txt", "w") as ofile:
                    cmd = f"./cmake-build/mininn -n 512 -l 0.005 -u {mu} -b {bs} -e 500 -P static -M eval -F paramtest_jsons"

                    print(f"Running '{cmd}' rep {ti}")
                    subprocess.call(cmd.split(" "), stdout=ofile)
