#include <iostream>

#include <andres/graph/complete-graph.hxx>
#include <andres/graph/hdf5/complete-graph.hxx>

#include "solve-mp.hxx"

int main(int argc, char* argv[])
try
{
    Parameters parameters;
    parseCommandLine(argc, argv, parameters);

    solveMulticutProblem<andres::graph::CompleteGraph<>>(parameters);

    return 0;
}
catch(const std::runtime_error& error)
{
    std::cerr << "error creating multicut problem: " << error.what() << std::endl;
    return 1;
}