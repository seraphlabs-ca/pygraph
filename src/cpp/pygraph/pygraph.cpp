/*
 * Implementation file of pygraph library.
 */

#include "pygraph.h"

#include <andres/functional.hxx>
#include <andres/graph/lifting.hxx>
#include <andres/graph/multicut/kernighan-lin.hxx>
#include <andres/graph/multicut-lifted/greedy-additive.hxx>
#include <andres/graph/multicut-lifted/kernighan-lin.hxx>

#include <utils.hxx>
#include <probabilistic-lifting.hxx>

using namespace std;
using namespace andres;
using namespace andres::graph;

///////////////////////////////////////////////////////////////////////////////
// Class GraphSolver
///////////////////////////////////////////////////////////////////////////////

GraphSolver::GraphSolver(int vert_num, bool weights_probabilities): 
    vert_num(vert_num), weights_probabilities(weights_probabilities) {
    this->graph.insertVertices(vert_num);
}

void GraphSolver::add_edge(int i, int j, double w) {
    if ((i >= this->vert_num) || (j >= this->vert_num)) {
        throw DEBUG_FORMAT_STR("Edges must be 0 <= i,j <= %i", this->vert_num);
    }

    this->graph.insertEdge(i, j); 
    this->weights.push_back(w);
}

std::vector<int> GraphSolver::kernighan_lin() {
    std::vector<char> edge_labels(this->graph.numberOfEdges());
    andres::graph::multicut::kernighanLin(this->graph, this->weights, edge_labels, edge_labels);

    return std::vector<int>(edge_labels.begin(), edge_labels.end());;
}

std::vector<int> GraphSolver::KLj(int distance_lower_bound, int distance_higher_bound) {
    if (distance_lower_bound < 0)
        distance_lower_bound = 0;
    if (distance_higher_bound < 0)
        distance_higher_bound = this->graph.numberOfVertices();

    // lift graph
    Graph<> lifted_graph;
    lift(graph, lifted_graph, distance_lower_bound, distance_higher_bound);

    vector<double> edge_cut_probabilities = this->weights;
    vector<double> edge_split_probabilities_lifted(lifted_graph.numberOfEdges());
    if (weights_probabilities)
        transform(
            edge_cut_probabilities.begin(),
            edge_cut_probabilities.end(),
            edge_cut_probabilities.begin(),
            ProbabilityToNegativeLogInverseProbability<double,double>()
        );
    
    liftEdgeValues(
        graph,
        lifted_graph,
        edge_cut_probabilities.begin(),
        edge_split_probabilities_lifted.begin()
    );
    
    if (weights_probabilities)
        transform(
            edge_split_probabilities_lifted.begin(),
            edge_split_probabilities_lifted.end(),
            edge_split_probabilities_lifted.begin(),
            NegativeLogProbabilityToInverseProbability<double,double>()
        );

    // Solve Lifted Multicut problem
    std::vector<char> edge_labels(lifted_graph.numberOfEdges());
    auto& edge_values = edge_split_probabilities_lifted;
    auto& original_graph = graph;

    // GAEC initialization
    andres::graph::multicut_lifted::greedyAdditiveEdgeContraction(original_graph, lifted_graph, edge_values, edge_labels);
    // kernighan-Lin optimization
    andres::graph::multicut_lifted::kernighanLin(original_graph, lifted_graph, edge_values, edge_labels, edge_labels);

    // read solution
    std::vector<int> vertex_labels(lifted_graph.numberOfVertices());
    edgeToVertexLabels(lifted_graph, edge_labels, vertex_labels);

    // TODO: use for initialization
    // Solution greedyAdditiveEdgeContraction(Problem<GRAPH> const& problem, Solution const& input_labeling)
    // TODO: use for final solution
    // Solution update_labels_and_multicut(Problem<GRAPH> const& problem, Solution const& input)

    return vertex_labels;
}

