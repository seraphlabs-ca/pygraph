/*
 * Implementation file of pygraph library.
 */

#include "pygraph.h"

#include <andres/functional.hxx>
#include <andres/graph/lifting.hxx>
#include <andres/graph/multicut/kernighan-lin.hxx>
#include <andres/graph/multicut-lifted/greedy-additive.hxx>
#include <andres/graph/multicut-lifted/kernighan-lin.hxx>
#include <nl-lmp/greedy-additive.hxx>
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
    vert_num = std::max(vert_num, std::max(i, j)+1);
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

    // check that we did not entered an edge twice
    if (graph->numberOfEdges() != this->weights.size()) {
        throw DEBUG_STR("An edge appeared more than once!");
    }
    return graph;
}
    

std::vector<int> GraphSolver::kernighan_lin() {
    std::shared_ptr < andres::graph::Graph<> > graph = this->get_graph();
    std::vector<char> edge_labels(graph->numberOfEdges());
    andres::graph::multicut::kernighanLin(*graph, this->weights, edge_labels, edge_labels);

    return std::vector<int>(edge_labels.begin(), edge_labels.end());
}

std::vector<int> GraphSolver::lmp_KL(int distance_lower_bound, int distance_higher_bound) {
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
    if (weights_probabilities) {
        transform(
            edge_cut_probabilities.begin(),
            edge_cut_probabilities.end(),
            edge_cut_probabilities.begin(),
            ProbabilityToNegativeLogInverseProbability<double,double>()
        );
    }    

    liftEdgeValues(
        *graph,
        lifted_graph,
        edge_cut_probabilities.begin(),
        edge_split_probabilities_lifted.begin()
    );
    
    if (weights_probabilities) {
        transform(
            edge_split_probabilities_lifted.begin(),
            edge_split_probabilities_lifted.end(),
            edge_split_probabilities_lifted.begin(),
            NegativeLogProbabilityToInverseProbability<double,double>()
        );
    }

    // Solve Lifted Multicut problem
    std::vector<char> edge_labels(lifted_graph.numberOfEdges());
    auto& edge_values = edge_split_probabilities_lifted;
    auto& original_graph = *graph;

    // convert probabilities to weights
    if (weights_probabilities) {
        std::transform(
            edge_values.begin(),
            edge_values.end(),
            edge_values.begin(),
            andres::NegativeLogProbabilityRatio<double,double>()
            );
    }


    // GAEC initialization
    andres::graph::multicut_lifted::greedyAdditiveEdgeContraction(original_graph, lifted_graph, edge_values, edge_labels);
    // kernighan-Lin optimization
    andres::graph::multicut_lifted::kernighanLin(original_graph, lifted_graph, edge_values, edge_labels, edge_labels);

    // read solution
    std::vector<int> vertex_labels(lifted_graph.numberOfVertices());
    edgeToVertexLabels(lifted_graph, edge_labels, vertex_labels);

    return vertex_labels;
}


std::vector< std::pair< int, int >  > GraphSolver::lmp_KLj(int distance_lower_bound, int distance_higher_bound) {
    std::shared_ptr < andres::graph::Graph<> > graph = this->get_graph();
    HERE;
    PRINT(weights.size());
    PRINT(graph->numberOfVertices())
    PRINT(graph->numberOfEdges())

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
    // PFORMAT_STR("graph.numberOfVertices() = %d", graph->numberOfVertices())
    // PFORMAT_STR("graph.numberOfEdges() = %d", graph->numberOfEdges())
    // PFORMAT_STR("lifted_graph.numberOfVertices() = %d", lifted_graph.numberOfVertices())
    // PFORMAT_STR("lifted_graph.numberOfEdges() = %d", lifted_graph.numberOfEdges())
    // PRINT("edge_cut_probabilities =")
    // PRINT_STD_VEC(edge_cut_probabilities)
    if (weights_probabilities) {
        transform(
            edge_cut_probabilities.begin(),
            edge_cut_probabilities.end(),
            edge_cut_probabilities.begin(),
            ProbabilityToNegativeLogInverseProbability<double,double>()
        );
        // PRINT("edge_cut_probabilities =")
        // PRINT_STD_VEC(edge_cut_probabilities)
    }    

    // HERE;
    liftEdgeValues(
        *graph,
        lifted_graph,
        edge_cut_probabilities.begin(),
        edge_split_probabilities_lifted.begin()
    );
    
    // PRINT("edge_split_probabilities_lifted =")
    // PRINT_STD_VEC(edge_split_probabilities_lifted)
    if (weights_probabilities) {
        transform(
            edge_split_probabilities_lifted.begin(),
            edge_split_probabilities_lifted.end(),
            edge_split_probabilities_lifted.begin(),
            NegativeLogProbabilityToInverseProbability<double,double>()
        );
        // PRINT("edge_split_probabilities_lifted =")
        // PRINT_STD_VEC(edge_split_probabilities_lifted)
    }

    // Solve Lifted Multicut problem
    std::vector<char> edge_labels(lifted_graph.numberOfEdges());
    auto& edge_values = edge_split_probabilities_lifted;
    auto& original_graph = *graph;

    // PRINT("egde_values =")
    // PRINT_STD_VEC(edge_values);
    // convert probabilities to weights
    if (weights_probabilities) {
        std::transform(
            edge_values.begin(),
            edge_values.end(),
            edge_values.begin(),
            andres::NegativeLogProbabilityRatio<double,double>()
            );
        // PRINT("egde_values =")
        // PRINT_STD_VEC(edge_values);
    }


    // GAEC initialization
    andres::graph::multicut_lifted::greedyAdditiveEdgeContraction(original_graph, lifted_graph, edge_values, edge_labels);
    // PRINT("egde_values =")
    // PRINT_STD_VEC(edge_values);
    // PRINT("egde_labels =")
    // PRINT_STD_VEC(std::vector<int>(edge_labels.begin(), edge_labels.end()));

    // build classes per vertex
    std::vector<int> vertex_labels(lifted_graph.numberOfVertices());
    edgeToVertexLabels(lifted_graph, edge_labels, vertex_labels);
    // PRINT("vertex_labels =")
    // PRINT_STD_VEC(vertex_labels);

    // get number of classes
    int num_classes = -1;
    for (auto& it : vertex_labels) {
        num_classes = std::max(num_classes, it+1);
    }
    // PFORMAT_STR("num_classes = %d", num_classes)

    nl_lmp::Problem< andres::graph::Graph<> > problem(vert_num, num_classes);
    nl_lmp::Solution input_labeling(vert_num);

    // build the problem from original
    for (int i = 0; i < edge_cut_probabilities.size(); i++) {
        int v0 = original_graph.vertexOfEdge(i, 0); 
        int v1 = original_graph.vertexOfEdge(i, 1); 
        int c0 = vertex_labels[v0]; 
        int c1 = vertex_labels[v1]; 

        // problem.setPairwiseCutCost(size_t v0, size_t v1, size_t c0, size_t c1, double value, bool add_edge_into_original_graph = true)
        problem.setPairwiseCutCost(v0, v1, c0, c1, edge_cut_probabilities[i], true);
    }

    // extend the problem from with lifted
    for (int i = 0; i < edge_split_probabilities_lifted.size(); i++) {
        int v0 = lifted_graph.vertexOfEdge(i, 0); 
        int v1 = lifted_graph.vertexOfEdge(i, 1); 

        int c0 = vertex_labels[v0]; 
        int c1 = vertex_labels[v1]; 

        // problem.setPairwiseCutCost(size_t v0, size_t v1, size_t c0, size_t c1, double value, bool add_edge_into_original_graph = true)
        problem.setPairwiseCutCost(v0, v1, c0, c1, edge_split_probabilities_lifted[i], false);
    }

    // store initial class labeling
    for (int i = 0; i < vert_num; i++) {
        input_labeling[i].classIndex = vertex_labels[i];
    }

    // Initialize clusters
    nl_lmp::Solution input_solution = nl_lmp::greedyAdditiveEdgeContraction(problem, input_labeling);
    // Run KLj algorithm
    nl_lmp::Solution output_solution = nl_lmp::update_labels_and_multicut(problem, input_solution);

    // store vertices class and clusters
    std::vector< std::pair< int, int >  > vertex_class_luster(vert_num);
    for (int i = 0; i < vert_num; i++) {
        vertex_class_luster[i].first = output_solution[i].classIndex;
        vertex_class_luster[i].second = output_solution[i].clusterIndex;
        // PFORMAT_STR("classIndex = %d clusterIndex = %d", output_solution[i].classIndex % output_solution[i].clusterIndex);
    }



    return vertex_class_luster;
}

