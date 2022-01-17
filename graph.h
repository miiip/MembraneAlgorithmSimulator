//
// Created by Mihail Plesa on 12.01.2022.
//

#ifndef CMAKE_BUILD_DEBUG_GRAPH_H
#define CMAKE_BUILD_DEBUG_GRAPH_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INF 10000000000.0
typedef struct vertex {
    int id;
    float weight;
}vertex;
typedef struct adjacent_vertices_list {
    vertex v;
    struct adjacent_vertices_list *next_node;
}adjacent_vertices_list;
adjacent_vertices_list* get_last_element_from_list(adjacent_vertices_list *vertices_list);
void add_vertext_to_vertices_list (adjacent_vertices_list **vertices_list, vertex v);
void build_graph_from_file(const char* file_name, adjacent_vertices_list*** graph);
void display_graph_vertex(int id, adjacent_vertices_list** graph);
float get_distance(int u, int v, adjacent_vertices_list** graph);
float compute_cost_of_tour(int* tour, int num_of_nodes, adjacent_vertices_list** graph);
void print_tour(int* tour, int num_of_nodes);
#endif //CMAKE_BUILD_DEBUG_GRAPH_H
