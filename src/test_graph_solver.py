import sys
sys.path.append("./cpp/build/pygraph")
import pygraphSWIG
import numpy as np

gs1 = pygraphSWIG.GraphSolver()
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

is_prob = True

if is_prob:
    def f(x):
        return 1.0 - x

else:
    def f(x):
        x = max(x, 1.0 / 255)
        x = min(x, 1.0 - 1.0 / 255)
        return -np.log(x / (1.0 - x))

gs2 = pygraphSWIG.GraphSolver()
# Need to use 1-prob to cut on low probability
gs2.add_edge(0, 1, f(1.0))
gs2.add_edge(0, 3, f(0.1))
gs2.add_edge(1, 2, f(0.9))
gs2.add_edge(1, 4, f(0.3))
gs2.add_edge(2, 5, f(0.1))
gs2.add_edge(3, 4, f(0.0))
gs2.add_edge(4, 5, f(0.8))
gs2.add_edge(4, 6, f(0.8))
gs2.add_edge(5, 6, f(0.8))

vertex_class_cluster = gs2.lmp_KLj()
print "vertex_class_cluster = %s" % str(vertex_class_cluster)
