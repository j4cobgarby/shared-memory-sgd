import json
import matplotlib.pyplot as plt

fig, host = plt.subplots(figsize=(8, 5), layout='constrained')

ax2 = host.twinx()
ax3 = host.twinx()

host.set_ylim(0, 3) # Model loss
ax2.set_ylim(0, 60) # Parallelism
ax3.set_ylim(-1, 1) # Loss gradient (smoothed)

host.set_xlabel("Time (seconds)")
host.set_ylabel("Model Loss")
ax2.set_ylabel("Parallelism")
ax3.set_ylabel("Delta loss")

plt.show()
