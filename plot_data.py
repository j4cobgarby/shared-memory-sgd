#!/usr/bin/env python3

import json
import os
import matplotlib.pyplot as plt
import matplotlib.colors as cols
from matplotlib.cm import get_cmap

import numpy as np

def plot_files(files):
    fig, ax1 = plt.subplots(figsize=(8, 5), layout='constrained')

    ax2 = ax1.twinx()
    ax3 = ax1.twinx()

    ax1.set_ylim(0, 3) # Model loss
    ax2.set_ylim(0, 60) # Parallelism
    #ax3.set_ylim(-1, 1) # Loss gradient (smoothed)

    ax1.set_xlabel("Time (seconds)")
    ax1.set_ylabel("Model Loss")
    ax2.set_ylabel("Parallelism")
    ax3.set_ylabel("Change in loss since previous execution phase")

    ax1.set_prop_cycle(color=get_cmap('Set1').colors)
    ax2.set_prop_cycle(color=get_cmap('Set2').colors)
    ax3.set_prop_cycle(color=get_cmap('tab10').colors)

    plots = []

    for fn in files:
        with open(fn, 'r') as file:
            dat = json.load(file)

            plots.append(ax1.plot(dat['epoch_time'], dat['epoch_loss'], label=f"Loss ({fn})"))
            plots.append(ax2.step(dat['mlist']['time'], dat['mlist']['m'], label=f"m ({fn})"))
            plots.append(ax3.plot(dat['lossgrad']['time'], dat['lossgrad']['grad'], label=f"ΔLoss ({fn})"))

    lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
    lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
    fig.legend(lines, labels)

    ax3.spines['left'].set_position(('outward', 60))
    ax3.spines['left'].set_visible(True)
    ax3.yaxis.set_label_position('left')
    ax3.yaxis.set_ticks_position('left')
    plt.show()

if __name__ == "__main__":
    files = [f for f in os.listdir('.') if f.endswith('.json')]

    plot_files(files)
