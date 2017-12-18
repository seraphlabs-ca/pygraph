/*
 * Implementation file of pygraph library.
 */

#include "pygraph.h"

#include <andres/functional.hxx>
#include <andres/graph/lifting.hxx>
#include <andres/graph/multicut/kernighan-lin.hxx>
#include <andres/graph/multicut-lifted/greedy-additive.hxx>
#include <andres/graph/multicut-lifted/kernighan-lin.hxx>
#include <nl-lmp/solve-joint.hxx>

#include <utils.hxx>
#include <probabilistic-lifting.hxx>


using namespace std;
using namespace andres;
using namespace andres::graph;

///////////////////////////////////////////////////////////////////////////////
// Class GraphSolver
///////////////////////////////////////////////////////////////////////////////

GraphSolver::GraphSolver(bool weights_probabilities): 
    vert_num(0), weights_probabilities(weights_probabilities) {
}

void GraphSolver::add_edge(int i, int j, double w) {
    if ((i < 0) || (j < 0)) {
        throw DEBUG_STR("Edges must be 0 <= i,j");
    }

    edges.push_back(std::tuple<int, int>(i, j));
    weights.push_back(w);
    // store number of vertices
    vert_num = std::max(vert_num, std::max(i, j));
}


// Returns a Graph with all edges
std::shared_ptr< andres::graph::Graph<> > GraphSolver::get_graph() {
    std::shared_ptr< andres::graph::Graph<> > graph = std::make_shared< andres::graph::Graph<> >();
    graph->insertVertices(vert_num);
    for (auto& it : this->edges) {
        int i, j;
        std::tie(i, j) = it;
        graph->insertEdge(i, j); 
    }

    return graph;
}
    


std::vector<int> GraphSolver::kernighan_lin() {
    std::shared_ptr < andres::graph::Graph<> > graph = this->get_graph();

    std::vector<char> edge_labels(graph->numberOfEdges());
    andres::graph::multicut::kernighanLin(*graph, this->weights, edge_labels, edge_labels);

    return std::vector<int>(edge_labels.begin(), edge_labels.end());
}

std::vector<int> GraphSolver::KLj(int distance_lower_bound, int distance_higher_bound) {
    std::shared_ptr < andres::graph::Graph<> > graph = this->get_graph();

    if (distance_lower_bound < 0) {
        distance_lower_bound = 0;
    }
    if (distance_higher_bound < 0) {
        distance_higher_bound = graph->numberOfVertices()-1;
    }

    // lift graph
    Graph<> lifted_graph;
    lift(*graph, lifted_graph, distance_higher_bound, distance_lower_bound);

    vector<double> edge_cut_probabilities = this->weights;
    vector<double> edge_split_probabilities_lifted(lifted_graph.numberOfEdges());
    PFORMAT_STR("graph.numberOfVertices() = %d", graph->numberOfVertices())
    PFORMAT_STR("graph.numberOfEdges() = %d", graph->numberOfEdges())
    PFORMAT_STR("lifted_graph.numberOfVertices() = %d", lifted_graph.numberOfVertices())
    PFORMAT_STR("lifted_graph.numberOfEdges() = %d", lifted_graph.numberOfEdges())
    PRINT("edge_cut_probabilities =")
    PRINT_STD_VEC(edge_cut_probabilities)
    if (weights_probabilities) {
        transform(
            edge_cut_probabilities.begin(),
            edge_cut_probabilities.end(),
            edge_cut_probabilities.begin(),
            ProbabilityToNegativeLogInverseProbability<double,double>()
        );
        PRINT("edge_cut_probabilities =")
        PRINT_STD_VEC(edge_cut_probabilities)
    }    

    HERE;
    liftEdgeValues(
        *graph,
        lifted_graph,
        edge_cut_probabilities.begin(),
        edge_split_probabilities_lifted.begin()
    );
    
    PRINT("edge_split_probabilities_lifted =")
    PRINT_STD_VEC(edge_split_probabilities_lifted)
    if (weights_probabilities) {
        transform(
            edge_split_probabilities_lifted.begin(),
            edge_split_probabilities_lifted.end(),
            edge_split_probabilities_lifted.begin(),
            NegativeLogProbabilityToInverseProbability<double,double>()
        );
        PRINT("edge_split_probabilities_lifted =")
        PRINT_STD_VEC(edge_split_probabilities_lifted)
    }

    // Solve Lifted Multicut problem
    std::vector<char> edge_labels(lifted_graph.numberOfEdges());
    auto& edge_values = edge_split_probabilities_lifted;
    auto& original_graph = *graph;

    PRINT("egde_values =")
    PRINT_STD_VEC(edge_values);
    if (weights_probabilities) {
        std::transform(
            edge_values.begin(),
            edge_values.end(),
            edge_values.begin(),
            andres::NegativeLogProbabilityRatio<double,double>()
            );
        PRINT("egde_values =")
        PRINT_STD_VEC(edge_values);
    }


    // GAEC initialization
    andres::graph::multicut_lifted::greedyAdditiveEdgeContraction(original_graph, lifted_graph, edge_values, edge_labels);
    PRINT("egde_values =")
    PRINT_STD_VEC(edge_values);
    PRINT("egde_labels =")
    PRINT_STD_VEC(std::vector<int>(edge_labels.begin(), edge_labels.end()));
    // kernighan-Lin optimization
    andres::graph::multicut_lifted::kernighanLin(original_graph, lifted_graph, edge_values, edge_labels, edge_labels);

    // read solution
    std::vector<int> vertex_labels(lifted_graph.numberOfVertices());
    edgeToVertexLabels(lifted_graph, edge_labels, vertex_labels);
    PRINT("vertex_labels =")
    PRINT_STD_VEC(vertex_labels);

    // TODO: use for initialization
    // Solution greedyAdditiveEdgeContraction(Problem<GRAPH> const& problem, Solution const& input_labeling)
    // TODO: use for final solution
    // Solution update_labels_and_multicut(Problem<GRAPH> const& problem, Solution const& input)

    return vertex_labels;
}

