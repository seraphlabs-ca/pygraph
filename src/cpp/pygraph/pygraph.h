/*
 * Header file of pygraph library.
 */

#ifndef __PYGRAPH_H__
#define __PYGRAPH_H__

#include "common/helper.h"

#include <andres/graph/graph.hxx>
#include <nl-lmp/solution.hxx>
#include <nl-lmp/problem.hxx>

#include <memory>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
// Classes
///////////////////////////////////////////////////////////////////////////////

class GraphSolver {
public:
    GraphSolver(bool weights_probabilities=true);
    void add_edge(int i, int j, double w=0.0);
    std::shared_ptr< andres::graph::Graph<> > get_graph();    
    std::vector<int> kernighan_lin();
    std::vector<int> lmp_KL(int distance_lower_bound=0, int distance_higher_bound=-1);
    std::vector< std::pair< int, int >  > mp_KLj();

protected:
    int vert_num;
    bool weights_probabilities;
    std::vector< std::tuple<int, int> > edges;
    std::vector< double > weights;
};

#endif //__PYGRAPH_H__
