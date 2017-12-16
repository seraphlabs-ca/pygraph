%include "common/common.i"

%apply (int DIM1, double* IN_ARRAY1)
{(int V_i, double * V)}
%apply (int* DIM1, double** ARGOUTVIEWM_ARRAY1)
{(int* V_i, double** V)}

%apply (int DIM1, int DIM2, double* IN_ARRAY2)
{(int V_i, int V_j, double * V)}
%apply (int* DIM1, int* DIM2, double** ARGOUTVIEWM_ARRAY2)
{(int* V_i, int* V_j, double** V)}

%apply (int DIM1, int* IN_ARRAY1)
{(int V_i, int* V)}
%apply (int* DIM1, int** ARGOUTVIEWM_ARRAY1)
{(int* V_i, int** V)}

%apply (int DIM1, int DIM2, int* IN_ARRAY2)
{(int V_i, int V_j, int* V)}
%apply (int* DIM1, int* DIM2, int** ARGOUTVIEWM_ARRAY2)
{(int* V_i, int* V_j, int** V)}

// %shared_ptr(Graph);

%include "pygraph/pygraph.h"

// %template(StringStringParameterType) BaseParameterType<std::string, std::string>;
