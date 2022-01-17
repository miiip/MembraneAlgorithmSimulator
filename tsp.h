//
// Created by Mihail Plesa on 12.01.2022.
//

#ifndef CMAKE_BUILD_DEBUG_TSP_H
#define CMAKE_BUILD_DEBUG_TSP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
void two_change(int* tour, int node1, int node2);
void two_opt_local_search(int** tour, int num_nodes, adjacent_vertices_list** graph);
void display_tour(int *tour, int num_of_nodes);
void generate_solution(int **tour, int num_nodes);
#endif //CMAKE_BUILD_DEBUG_TSP_H
