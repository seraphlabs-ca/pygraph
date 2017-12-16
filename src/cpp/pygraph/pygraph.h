/*
 * Header file of pygraph library.
 */

#ifndef __PYGRAPH_H__
#define __PYGRAPH_H__

#include "common/helper.h"

#include <andres/graph/graph.hxx>
#include "andres/graph/multicut/kernighan-lin.hxx"

///////////////////////////////////////////////////////////////////////////////
// Classes
///////////////////////////////////////////////////////////////////////////////

class GraphSolver {
public:
    void add_edge(int i, int j, double w=0.0);
    std::vector<char> kernighan_lin();

protected:
    andres::graph::Graph<> graph;
    std::vector<double> weights;
};

#endif //__PYGRAPH_H__
