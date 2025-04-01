#!/usr/bin/env python3

import sys
import json

# Takes a number of experiment files as arguments, and outputs to stdout a new file with average loss and epoch times
# It's assumed that:
#  - Each input has the same number of data points

if __name__ == "__main__":
    dats = []

    for f in sys.argv[1:]:
        with open(f, "r") as file:
            dat = json.load(file)
            dats.append(dat)

    out = {}

    out["epoch_loss"] = [
        sum([d["epoch_loss"][i] for d in dats]) / len(dats)
        for i in range(len(dats[0]["epoch_loss"]))
    ]
    out["epoch_time"] = [
        sum([d["epoch_time"][i] for d in dats]) / len(dats)
        for i in range(len(dats[0]["epoch_time"]))
    ]
    out["epoch_min_losses"] = [
        min([d["epoch_loss"][i] for d in dats])
        for i in range(len(dats[0]["epoch_loss"]))
    ]
    out["epoch_max_losses"] = [
        max([d["epoch_loss"][i] for d in dats])
        for i in range(len(dats[0]["epoch_loss"]))
    ]

    print(json.dumps(out))
