/*
 * Implementation file of pygraph library.
 */

#include "pygraph.h"


///////////////////////////////////////////////////////////////////////////////
// Class GraphSolver
///////////////////////////////////////////////////////////////////////////////

void GraphSolver::add_edge(int i, int j, double w) {
    this->graph.insertVertices(1);
    this->graph.insertEdge(i, j); 
    this->weights.push_back(w);
}

std::vector<char> GraphSolver::kernighan_lin() {
    std::vector<char> edge_labels(this->graph.numberOfEdges());
    andres::graph::multicut::kernighanLin(this->graph, this->weights, edge_labels, edge_labels);

    return edge_labels;
}

