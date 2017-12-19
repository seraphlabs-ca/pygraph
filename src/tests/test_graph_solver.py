import pygraph
import numpy as np
import time

# test KL
gs1 = pygraph.GraphSolver()
gs1.add_edge(0, 1, 5.0)
gs1.add_edge(0, 3, -20.0)
gs1.add_edge(1, 2, 5.0)
gs1.add_edge(1, 4, 5.0)
gs1.add_edge(2, 5, -20.0)
gs1.add_edge(3, 4, 5.0)
gs1.add_edge(4, 5, 5.0)


edge_label = gs1.kernighan_lin()
print "edge_label = %s" % str(edge_label)

if edge_label != (0, 1, 0, 1, 1, 0, 0):
    print "TEST FAILED"
else:
    print "TEST PASSED"


def get_gs(is_prob=True):
    if is_prob:
        def f(x):
            return 1.0 - x

    else:
        def f(x):
            x = max(x, 1.0 / 255)
            x = min(x, 1.0 - 1.0 / 255)
            return np.log(x / (1.0 - x))

    # Test lifting multicut problem + KL
    gs = pygraph.GraphSolver(is_prob)
    # Probability to cut edge
    gs.add_edge(0, 1, f(1.0))
    gs.add_edge(0, 3, f(0.1))
    gs.add_edge(1, 2, f(0.9))
    gs.add_edge(1, 4, f(0.3))
    gs.add_edge(2, 5, f(0.1))
    gs.add_edge(3, 4, f(0.0))
    gs.add_edge(4, 5, f(0.8))
    gs.add_edge(4, 6, f(0.8))
    gs.add_edge(5, 6, f(0.8))

    return gs

gs21 = get_gs(True)
vertex_cluster = gs21.lmp_KL()
print "lmp_KL vertex_cluster = %s" % str(vertex_cluster)

gs22 = get_gs(False)
vertex_class_cluster = gs22.mp_KLj()
print "mp_KLj vertex_class_cluster = %s" % str(vertex_class_cluster)

if list(vertex_cluster) != [v[1] for v in vertex_class_cluster]:
    print "is_prob FAILED"
else:
    print "is_prob PASSED"

# test speed

# 10 frames
N = 10
# 5 subjects
S = 5
# R repeats
R = 10

t0 = time.time()

for r in range(R):
    gs3 = pygraph.GraphSolver()
    for i in range(N):
        for s in range(S):
            # connect to all future frames and all subjects
            for j in range(i, N):
                for t in range(S):
                    if (i != j) or (s > t):
                        gs3.add_edge(i * N + s, j * N + t, np.random.rand())
    gs3.mp_KLj()

t1 = time.time()

dt = (t1 - t0) / R
print "mp_KLj %.2e [sec] = %.2e [fps]" % (dt, 1.0 / dt)
