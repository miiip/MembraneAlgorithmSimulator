//
// Created by Mihail Plesa on 12.01.2022.
//

#ifndef CMAKE_BUILD_DEBUG_MEMBRANE_H
#define CMAKE_BUILD_DEBUG_MEMBRANE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tsp.h"
typedef struct region {
    int num_of_solutions;
    int **solutions;
}region;
void initialize_regions(region **regions, int num_of_regions, int *num_of_solutions, int *** solutions, int num_of_nodes);
void initialize_regions_from_file(region **regions, int *num_of_regions, int num_of_nodes, const char *config_file);
void display_region(region region, int num_of_nodes, int index, adjacent_vertices_list** graph);
void display_regions(region * regions, int num_of_regions, int num_of_nodes, adjacent_vertices_list** graph);
void remove_solution_from_region(region *region, int solution_index, int num_of_nodes);
void add_solution_to_region(region *region, int* solution, int num_of_nodes);
void move_solution(region **regions, int initial_region, int final_region, int solution_index, int num_of_nodes);
void generate_random_regions_configurations(region **regions, int num_of_regions, int *num_of_solutions, int num_of_nodes);
void evolve_region(region* region, int num_of_nodes, adjacent_vertices_list** graph);
#endif //CMAKE_BUILD_DEBUG_MEMBRANE_H
