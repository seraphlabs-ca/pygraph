/*
 * Header file of pygraph library.
 */

#ifndef __PYGRAPH_H__
#define __PYGRAPH_H__

#include "common/helper.h"

#include <andres/graph/graph.hxx>

///////////////////////////////////////////////////////////////////////////////
// Classes
///////////////////////////////////////////////////////////////////////////////

class GraphSolver {
public:
    GraphSolver(int vert_num, bool weights_probabilities=true);
    void add_edge(int i, int j, double w=0.0);
    std::vector<int> kernighan_lin();
    std::vector<int> KLj(int distance_lower_bound=0, int distance_higher_bound=-1);

protected:
    int vert_num;
    bool weights_probabilities;
    andres::graph::Graph<> graph;
    std::vector<double> weights;
};

#endif //__PYGRAPH_H__
