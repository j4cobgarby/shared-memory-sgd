#!/usr/bin/env python3

import json
import os
import matplotlib.pyplot as plt
import matplotlib.colors as cols
import matplotlib

import numpy as np

import sys


def plot_files(files):
    fig, ax1 = plt.subplots(figsize=(8, 5), layout="constrained")

    ax2 = ax1.twinx()
    #ax3 = ax1.twinx()

    ax2.set_xlabel("Time (seconds)")
    ax2.set_ylabel("Model Loss")
    ax1.set_ylabel("Parallelism")
    #ax3.set_ylabel("Change in loss since previous execution phase")

    ax1.set_prop_cycle(color=matplotlib.colormaps["tab10"].colors)
    ax2.set_prop_cycle(color=matplotlib.colormaps["tab10"].colors)
    #ax3.set_prop_cycle(color=matplotlib.colormaps["Pastel1"].colors)

    plots = []

    for fn in files:
        with open(fn, "r") as file:
            dat = json.load(file)

            if "meta" in dat:
                print(fn, json.dumps(dat["meta"], sort_keys=True, indent=2))

            if "mlist" in dat:
                if "probe_starts" in dat["mlist"]:
                    plots.append(
                        ax1.vlines(
                            [
                                dat["mlist"]["time"][i]
                                for i in dat["mlist"]["probe_starts"]
                            ],
                            min(dat["mlist"]["m"]),
                            max(dat["mlist"]["m"]),
                        )
                    )
                if "probe_ends" in dat["mlist"]:
                    plots.append(
                        ax1.vlines(
                            [
                                dat["mlist"]["time"][i]
                                for i in dat["mlist"]["probe_ends"]
                            ],
                            min(dat["mlist"]["m"]),
                            max(dat["mlist"]["m"]),
                        )
                    )
                plots.append(
                    ax1.step(
                        dat["mlist"]["time"],
                        dat["mlist"]["m"],
                        "--",
                        where="post",
                        label=f"m ({fn})",
                    )
                )
            # if "lossgrad" in dat:
            #     plots.append(
            #         ax3.plot(
            #             dat["lossgrad"]["time"],
            #             dat["lossgrad"]["grad"],
            #             label=f"ΔLoss ({fn})",
            #             marker="o",
            #             ms=3,
            #         )
            #     )

            if "epoch_min_losses" and "epoch_max_losses" in dat:
                errs_lo = list(
                    map(
                        lambda t: abs(t[0] - t[1]),
                        zip(dat["epoch_loss"], dat["epoch_min_losses"]),
                    )
                )
                errs_hi = list(
                    map(
                        lambda t: abs(t[0] - t[1]),
                        zip(dat["epoch_loss"], dat["epoch_max_losses"]),
                    )
                )
                plots.append(
                    ax2.errorbar(
                        dat["epoch_time"],
                        dat["epoch_loss"],
                        yerr=[errs_lo, errs_hi],
                        label=f"Loss ({fn})",
                        marker="",
                        ms=3,
                        capsize=1.5,
                        elinewidth=1,
                        errorevery=4,
                    )
                )
            else:
                plots.append(
                    ax2.plot(
                        dat["epoch_time"],
                        dat["epoch_loss"],
                        label=f"Loss ({fn})",
                        marker="",
                        ms=3,
                    )
                )

    lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
    lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
    fig.legend(lines, labels)

    ax1.yaxis.set_label_position("right")
    ax1.yaxis.set_ticks_position("right")
    ax2.yaxis.set_label_position("left")
    ax2.yaxis.set_ticks_position("left")

    # ax3.spines["left"].set_position(("outward", 60))
    # ax3.spines["left"].set_visible(True)
    # ax3.yaxis.set_label_position("left")
    # ax3.yaxis.set_ticks_position("left")
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) == 1:  # read all jsons in cwd
        files = [f for f in os.listdir(".") if f.endswith(".json")]
        plot_files(files)
    # elif len(sys.argv) == 2: # arg is a directory, read all jsons in it
    #     exp_dir = sys.argv[1]
    #     files = [exp_dir + "/" + f for f in os.listdir(exp_dir) if f.endswith('.json')]
    #     plot_files(files)
    else:  # all args are (json) files
        plot_files(sys.argv[1:])
