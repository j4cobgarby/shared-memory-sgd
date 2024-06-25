def read_csv(name):
    ret = {}
    with open(name, "r") as f:
        for ln in f.readlines():
            print(ln)
            _, n, loss = ln.split(",")
            ret[int(n)] = float(loss)
    return ret

def plot_csv(name):
    dic = read_csv(name)
    xs = dic.keys()
    ys = dic.values()

    import matplotlib.pyplot as plt

    plt.plot(xs, ys)
    plt.show()
