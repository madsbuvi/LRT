#ifndef host_tracer_h
#define host_tracer_h
#include "vtypes.h"
#include "clstuff.h"
#include "shared.h"
#include "geometry.h"

int selectTrace( Ray ray, int* ints, float* floats, int n );
int selectTrace2( Ray ray, std::vector<Geometry*>& geometry );

#endif