import sys
sys.path.append("./cpp/build/pygraph")
import pygraphSWIG

gs = pygraphSWIG.GraphSolver(6)
gs.add_edge(0, 1, 5.0)
gs.add_edge(0, 3, -20.0)
gs.add_edge(1, 2, 5.0)
gs.add_edge(1, 4, 5.0)
gs.add_edge(2, 5, -20.0)
gs.add_edge(3, 4, 5.0)
gs.add_edge(4, 5, 5.0)


edge_label = gs.kernighan_lin()
print "edge_label = %s" % str(edge_label)

if edge_label != (0, 1, 0, 1, 1, 0, 0):
    print "TEST FAILED"
else:
    print "TEST PASSED"
