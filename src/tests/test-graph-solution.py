"""
Tests pygraph solver solution.
"""

import pygraph
import numpy as np
import time

#=============================================================================#
# Global variables
#=============================================================================#


def build_trajectories(N, K, H=4):
    """
    N - sequence length
    K - trajectory number
    H - harmonics in trajectory

    Returns:
        T [K x N x 2] matrix of 2D trajectory
    """
    T = []

    n = np.linspace(0, 1.8 * np.pi, N).reshape((-1, 1))
    # create K trajectories
    for k in range(K):
        r = (np.random.randn(H) * 1.0 / np.arange(1, H + 1)**2).reshape((1, -1))
        h = (np.arange(H)).reshape((1, -1))
        x = np.sum(r * np.cos(n * h), axis=1)
        y = np.sum(r * np.sin(n * h), axis=1)
        T.append([x, y])

    # create T
    T = np.array(T).transpose([0, 2, 1])

    # normalize to [0, 1] coordinates
    T = T - np.mean(np.mean(T, axis=0), axis=0)
    T = T / (np.max(T) - np.min(T) + 1e-8)

    return T


def plot_trajectories(T, C):
    """
    Plots the trajectories.
    """
    K = T.shape[0]
    colors = plt.get_cmap("jet")(np.linspace(0.0, 1.0, K))

    fig = plt.figure()
    # plot 2D trajectories
    for k, XY in enumerate(T):
        c = colors[k]
        plt.plot(XY[:, 0], XY[:, 1], marker="x", lw=1, label="%i" % (k + 1))

    plt.title("T")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid()
    plt.legend(loc="best")
    plt.tight_layout()

    # plot time series
    fig = plt.figure()
    for k, XY in enumerate(T):
        plt.plot(XY[:, 0], marker="x", c=colors[k], lw=1, label="%i" % (k + 1))
        plt.plot(XY[:, 1], marker="o", c=colors[k], lw=1)

    plt.title("Time Series T")
    plt.xlabel("step")
    plt.ylabel("value")
    plt.grid()
    plt.legend(loc="best")
    plt.tight_layout()

    # plot trajectories based on clusters
    fig = plt.figure()

    K = np.max(C) + 1
    colors = plt.get_cmap("jet")(np.linspace(0.0, 1.0, K))
    # plot 2D trajectories
    for k in range(K):
        XY = T[C == k]
        c = colors[k]
        plt.scatter(XY[:, 0], XY[:, 1], marker="x", lw=1, label="%i" % (k + 1))

    plt.title("C")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid()
    plt.legend(loc="best")
    plt.tight_layout()


def get_clusters(T, k_weight=1000.0, d_weight=1.0, is_prob=True):
    """
    Builds a graph from T, and cluster based on graph multicut
    """
    # test KL
    gs = pygraph.GraphSolver(is_prob)

    added_e = set()
    # maps v -> (k, n)
    kn_v_map = {}
    v_kn_map = {}

    def get_vertex(k, n):
        """
        Create a unique vertex for (k, n)
        """
        if (k, n) not in kn_v_map:
            v = len(kn_v_map)
            kn_v_map[(k, n)] = v
            v_kn_map[v] = (k, n)

        return kn_v_map[(k, n)]

    K, N = T.shape[0:2]

    W = np.zeros((K, N), dtype=np.int)

    # connect all observations with all observations
    for k1 in range(K):
        for n1 in range(N):
            v1 = get_vertex(k1, n1)
            for k2 in range(k1, K):
                for n2 in range(n1, N):
                    v2 = get_vertex(k2, n2)

                    # add each edge only once
                    e = (v1, v2) if v1 >= 2 else (v2, v1)
                    # we add edge only once
                    if (e not in added_e) and (v1 != v2):
                        added_e.add(e)
                        xy1 = T[k1, n1]
                        xy2 = T[k2, n2]
                        # use k and distance as weight
                        c = k_weight * np.power(float(k1 - k2) / K, 2) + d_weight * np.sum(np.power(xy1 - xy2, 2))
                        # print "v1 = %i v2 = %i c = %.2e" % (v1, v2, c)
                        gs.add_edge(v1, v2, 1.0 - exp(-c))

    print "edges = %i vertices = %i" % (len(added_e), len(kn_v_map))

    # cluster T

    # vertex_cluster = gs.lmp_KL()
    # print "vertex_cluster = %s" % str(vertex_cluster)

    vertex_class_cluster = gs.mp_KLj()
    vertex_class, vertex_cluster = zip(*vertex_class_cluster)
    print "vertex_class = %s" % str(vertex_class)
    print "vertex_cluster = %s" % str(vertex_cluster)

    C = np.zeros((K, N), dtype=np.int)
    for v, c in enumerate(vertex_cluster):
        k, n = v_kn_map[v]
        C[k, n] = c

    return C


#=============================================================================#
# Main
#=============================================================================#
if __name__ == "__main__":
    N = 20
    K = 5

    T = build_trajectories(N=N, K=K)

    C = get_clusters(T=T)

    print "C = %s" % str(C)
    print "C trajectory is correct = %s" % str(np.all(C == C[:, 0:1], axis=1))

    plot_trajectories(T, C)
