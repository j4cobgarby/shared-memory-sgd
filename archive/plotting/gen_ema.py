import json
import sys

def read_json(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
        return data['epoch_loss'], data['epoch_time'], data['staleness_dist']

def calc_ema(data, smoothing):
    res = [data[0]]

    for point in data:
        res.append(smoothing * point + (1-smoothing) * res[-1])

    return res[1:] # trim off first point, which was just initial average

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} input_json out_filename")
        sys.exit(1)
    
    loss, times, staleness = read_json(sys.argv[1])

    loss_ema = calc_ema(loss, 0.3)

    assert (len(loss) == len(times) == len(loss_ema))

    with open(sys.argv[2], "w") as outf:
        outf.write(json.dumps({'avg': loss_ema, 'times': times}))
