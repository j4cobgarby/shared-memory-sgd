import json
import os
import matplotlib.pyplot as plt

def plot_epoch_loss_vs_time(ax, epoch_loss, epoch_time, label):
    ax.plot(epoch_time, epoch_loss, label=label)
    ax.set_xlabel('Epoch Time (seconds)')
    ax.set_ylabel('Epoch Loss')
    ax.set_title('Epoch Loss vs Epoch Time')
    ax.legend()

def plot_parallelism(ax, mlist_data, label):
    ax2 = ax.twinx()
    ax2.step([entry['time'] for entry in mlist_data], [entry['m'] for entry in mlist_data], label=label, color='black', where='post', linewidth=0.5)
    # ax2.scatter([entry['time'] for entry in mlist_data], [entry['m'] for entry in mlist_data], color='black', s=1)  # Scatter plot with circles
    ax2.set_ylabel('Parallelism (m)')
    ax2.set_ylim([0,60])
    ax2.legend(loc='upper left')

def read_json(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
        return data['epoch_loss'], data['epoch_time']

def read_mlist(file_path):
    with open(file_path, 'r') as file:
        mlist_data = json.load(file)
        return mlist_data

def plot_all_json_files():
    files = [f for f in os.listdir('.') if f.endswith('.json')]

    # Create a 1x1 subplot grid
    fig, ax = plt.subplots(1, 1, figsize=(8, 4))

    for file in files:
        if file.startswith('mlist'):
            mlist_data = read_mlist(file)
            plot_parallelism(ax, mlist_data, label=file)
        else:
            epoch_loss, epoch_time = read_json(file)
            plot_epoch_loss_vs_time(ax, epoch_loss, epoch_time, label=file)

    plt.tight_layout()  # Adjust layout to prevent clipping
    plt.show()

if __name__ == "__main__":
    plot_all_json_files()
