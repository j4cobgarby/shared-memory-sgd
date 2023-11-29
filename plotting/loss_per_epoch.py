import json
import os
import matplotlib.pyplot as plt

def plot_epoch_loss(ax, epoch_loss, label):
    ax.plot(epoch_loss, label=label)
    ax.set_xlabel('Epoch Index')
    ax.set_ylabel('Epoch Loss')
    ax.set_title('Comparison of Epoch Loss')
    ax.legend()

def plot_epoch_loss_vs_time(ax, epoch_loss, epoch_time, label):
    ax.plot(epoch_time, epoch_loss, label=label)
    ax.set_xlabel('Epoch Time (seconds)')
    ax.set_ylabel('Epoch Loss')
    ax.set_title('Epoch Loss vs Epoch Time')
    ax.legend()

def read_json(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
        return data['epoch_loss'], data['epoch_time']

def plot_all_json_files():
    files = [f for f in os.listdir('.') if f.endswith('.json')]

    # Create a 1x2 subplot grid
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4))

    for file in files:
        epoch_loss, epoch_time = read_json(file)
        plot_epoch_loss(ax1, epoch_loss, label=file)
        plot_epoch_loss_vs_time(ax2, epoch_loss, epoch_time, label=file)

    plt.tight_layout()  # Adjust layout to prevent clipping
    plt.show()

if __name__ == "__main__":
    plot_all_json_files()
