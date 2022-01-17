//
// Created by Mihail Plesa on 12.01.2022.
//

#include "tsp.h"

void two_change(int* tour, int node1, int node2) {
    int num_of_swaps = (node2 - node1 + 1)/2;
    for(int i=0; i<num_of_swaps; i++) {
        int tmp = tour[i + node1];
        tour[i + node1] = tour[node2 - i];
        tour[node2 - i] = tmp;
    }

}

void two_opt_local_search(int** tour, int num_nodes, adjacent_vertices_list** graph) {

    float cost = compute_cost_of_tour((*tour), num_nodes, graph);
    for(int node1=0; node1< num_nodes - 1; node1++) {
        for(int node2=node1+1; node2<num_nodes-1; node2++) {
            int *new_tour = (int *) malloc(num_nodes * sizeof(int));
            memcpy(new_tour, (*tour), num_nodes * sizeof(int));

            two_change(new_tour, node1, node2);

            float new_cost = compute_cost_of_tour(new_tour, num_nodes, graph);
            if (new_cost < cost) {
                (*tour) = new_tour;
            }

        }
    }

}
void display_tour(int *tour, int num_of_nodes) {
    printf("Printing the tour ... \n");
    for(int i=0; i<num_of_nodes; i++) {
        printf("%d ", tour[i]);
    }
    printf("\n");
}
void generate_solution(int **tour, int num_nodes) {
    int in, im;
    (*tour) = (int*)malloc(num_nodes * sizeof(int));
    for(int i=0; i<num_nodes; i++) {
        (*tour)[i] = i;
    }
    for (int i = num_nodes - 1; i >= 0; --i){

        int j = rand() % (i+1);

        int temp = (*tour)[i];
        (*tour)[i] = (*tour)[j];
        (*tour)[j] = temp;
    }
}