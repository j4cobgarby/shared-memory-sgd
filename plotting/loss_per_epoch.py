import json
import os
import matplotlib.pyplot as plt

def plot_epoch_loss_vs_time(ax, epoch_loss, epoch_time, label):
    ax.plot(epoch_time, epoch_loss, label=label)
    # ax.scatter(epoch_time, epoch_loss, color='black', s=5)
    ax.set_xlabel('Epoch Time (seconds)')
    ax.set_ylabel('Epoch Loss')
    ax.set_title('Epoch Loss vs Epoch Time')
    ax.legend()

def plot_avg(ax, data, times, label):
    ax.plot(times, data, label=label)

def plot_staleness(ax, staleness_dist, epoch_time, label):
    ax.plot(epoch_time, staleness_dist, label="Staleness for " + label)

def plot_parallelism(ax, mlist_data, label):
    ax.step([entry['time'] for entry in mlist_data], [entry['m'] for entry in mlist_data], label=label, where='post', linewidth=0.5)
    # ax2.scatter([entry['time'] for entry in mlist_data], [entry['m'] for entry in mlist_data], color='black', s=1)  # Scatter plot with circles
    ax.legend()

def read_json(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
        return data['epoch_loss'], data['epoch_time'], data['staleness_dist']

def read_mlist(file_path):
    with open(file_path, 'r') as file:
        mlist_data = json.load(file)
        return mlist_data

def read_avg(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
        return data['avg'], data['times']

def plot_all_json_files():
    files = [f for f in os.listdir('.') if f.endswith('.json')]

    # Create a 1x1 subplot grid
    fig, ax = plt.subplots(1, 1, figsize=(8, 4))
    twinaxis = ax.twinx()
    twinaxis.legend(loc='upper left')
    twinaxis.set_ylim([0, 60])
    twinaxis.set_ylabel('Parallelism (m)')
    

    for file in files:
        if file.startswith('mlist'):
            mlist_data = read_mlist(file)
            plot_parallelism(twinaxis, mlist_data, label=file)
        elif file.startswith('avg'):
            avg_data, avg_times = read_avg(file)
            print('avg data', avg_data)
            plot_avg(ax, avg_data, avg_times, label=file)
        else:
            epoch_loss, epoch_time, staleness_dist = read_json(file)
            plot_epoch_loss_vs_time(ax, epoch_loss, epoch_time, label=file)
            # plot_staleness(ax, staleness_dist, epoch_time, label=file)

    plt.tight_layout()  # Adjust layout to prevent clipping
    plt.show()

if __name__ == "__main__":
    plot_all_json_files()
