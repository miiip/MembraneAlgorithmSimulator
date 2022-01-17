//
// Created by Mihail Plesa on 12.01.2022.
//

#include "graph.h"
adjacent_vertices_list* get_last_element_from_list(adjacent_vertices_list *vertices_list) {
    adjacent_vertices_list *p = vertices_list;
    while(p->next_node != NULL) {
        p = p->next_node;
    }
    return p;

}

void add_vertext_to_vertices_list (adjacent_vertices_list **vertices_list, vertex v) {
    if((*vertices_list) == NULL) {
        (*vertices_list) = (adjacent_vertices_list*)malloc(sizeof(adjacent_vertices_list));
        (*vertices_list)->v.id = v.id;
        (*vertices_list)->v.weight = v.weight;
        (*vertices_list)->next_node = NULL;
        return;
    }

    adjacent_vertices_list *new_node = (adjacent_vertices_list*)malloc(sizeof(adjacent_vertices_list));
    new_node->v.id = v.id;
    new_node->v.weight = v.weight;
    new_node->next_node = NULL;
    adjacent_vertices_list *last_node = get_last_element_from_list((*vertices_list));

    last_node->next_node = new_node;
}
void build_graph_from_file(const char* file_name, adjacent_vertices_list*** graph) {
    FILE *f = fopen(file_name, "r");
    if(f==NULL) {
        exit(1);
    }
    char * line = NULL;
    size_t len = 0;
    int counter = 0;
    while (getline(&line, &len, f) != -1) {
        char *token = strtok(line, " \n");
        int id = 0;
        while(token != NULL) {
            int weight = atoi(token);
            vertex v;
            v.weight = weight;
            v.id = id;
            add_vertext_to_vertices_list(&(*graph)[counter], v);
            token = strtok(NULL, " \n");
            id++;
        }
        counter++;
    }
    fclose(f);
}
void display_graph_vertex(int id, adjacent_vertices_list** graph) {
    adjacent_vertices_list *node = graph[id];
    while(node != NULL) {
        printf("%.1f ", node->v.weight);
        node = node->next_node;
    }
}
float get_distance(int u, int v, adjacent_vertices_list** graph) {
    adjacent_vertices_list *p = graph[u];
    while(p != NULL) {
        if(p->v.id == v) {
            return p->v.weight;
        }
        p=p->next_node;
    }
    return INF;
}
float compute_cost_of_tour(int* tour, int num_of_nodes, adjacent_vertices_list** graph) {
    float cost = 0;
    for(int i=0; i<num_of_nodes - 1; i++) {
        cost = cost + get_distance(tour[i], tour[i+1], graph);
    }
    cost = cost + get_distance(tour[num_of_nodes - 1], tour[0], graph);
    return cost;
}
void print_tour(int* tour, int num_of_nodes) {
    for(int i=0; i<num_of_nodes; i++) {
        printf("%d ", tour[i]);
    }
    printf("\n");
}