//
// Created by Mihail Plesa on 12.01.2022.
//
#include "membrane.h"
#include "tsp.h"
void initialize_regions(region **regions, int num_of_regions, int *num_of_solutions, int *** solutions, int num_of_nodes) {
    (*regions) = (region*)malloc(num_of_regions * sizeof(region));
    for(int i=0; i<num_of_regions; i++) {
        (*regions)[i].num_of_solutions = num_of_solutions[i];
        (*regions)[i].solutions = (int**)malloc(num_of_solutions[i] * sizeof(int*));
    }
    for(int i=0; i<num_of_regions; i++) {
        for(int j=0; j<num_of_solutions[i]; j++) {
            (*regions)[i].solutions[j] = (int*)malloc(num_of_nodes * sizeof(int));
            memcpy((*regions)[i].solutions[j], solutions[i][j], num_of_nodes * sizeof(int));
        }
    }
}
void initialize_regions_from_file(region **regions, int *num_of_regions, int num_of_nodes, const char *config_file) {
    FILE *f = fopen(config_file, "r");
    if(f==NULL) {
        exit(1);
    }
    fscanf(f, "%d", num_of_regions);
    (*regions) = (region*)malloc((*num_of_regions) * sizeof(region));
    for(int i=0; i<(*num_of_regions); i++) {
        fscanf(f, "%d", &(*regions)[i].num_of_solutions);
        (*regions)[i].solutions = (int**)malloc((*regions)[i].num_of_solutions * sizeof(int*));
        for(int j=0; j<(*regions)[i].num_of_solutions; j++) {
            (*regions)[i].solutions[j] = (int *) malloc(num_of_nodes * sizeof(int));
        }
    }
    for(int i=0; i<(*num_of_regions); i++) {
        for(int j=0; j<(*regions)[i].num_of_solutions; j++) {
            for(int k=0; k<num_of_nodes; k++) {
                fscanf(f, "%d", &(*regions)[i].solutions[j][k]);
            }
        }
    }
}
void generate_random_regions_configurations(region **regions, int num_of_regions, int *num_of_solutions, int num_of_nodes) {
    int ***solutions = (int***)malloc(num_of_regions * sizeof(int**));
    for(int i=0; i<num_of_regions; i++) {
        solutions[i] = (int**)malloc(num_of_solutions[i] * sizeof(int*));
    }
    for(int i=0; i<num_of_regions; i++) {
        for(int j=0; j<num_of_solutions[i]; j++) {
            generate_solution(&solutions[i][j], num_of_nodes);
        }
    }
    initialize_regions(regions, num_of_regions, num_of_solutions, solutions, num_of_nodes);

}
void display_region(region region, int num_of_nodes, int index, adjacent_vertices_list** graph) {
    printf("############### Region #%d ##################\n", index);
    printf("Number of solutions: %d\n", region.num_of_solutions);
    printf("Solutions:\n");

    for(int j=0; j<region.num_of_solutions; j++) {
        for(int k=0; k<num_of_nodes; k++) {
            printf("%d ", region.solutions[j][k]);
        }
        printf("cost=%.2f", compute_cost_of_tour(region.solutions[j], num_of_nodes, graph));
        printf("\n");
    }
    printf("#################################\n");
}
void display_regions(region * regions, int num_of_regions, int num_of_nodes, adjacent_vertices_list** graph) {
    for(int i=0; i<num_of_regions; i++) {
        display_region(regions[i], num_of_nodes, i, graph);
    }
}
void remove_solution_from_region(region *region, int solution_index, int num_of_nodes) {
    int **new_solutions = (int**)malloc(((*region).num_of_solutions - 1 ) * sizeof(int*));
    for(int i=0; i<(*region).num_of_solutions - 1; i++) {
        new_solutions[i] = (int*)malloc(num_of_nodes * sizeof(int));
    }
    for(int i=0; i<solution_index; i++) {
        memcpy(new_solutions[i], (*region).solutions[i], num_of_nodes*sizeof(int));
    }
    for(int i=solution_index+1; i<(*region).num_of_solutions; i++) {
        memcpy(new_solutions[i-1], (*region).solutions[i], num_of_nodes*sizeof(int));
    }
    for(int i=0; i<(*region).num_of_solutions; i++) {
        free((*region).solutions[i]);
    }
    free((*region).solutions);
    (*region).solutions = new_solutions;
    (*region).num_of_solutions--;
}
void add_solution_to_region(region *region, int* solution, int num_of_nodes) {
    int **new_solutions = (int**)malloc(((*region).num_of_solutions + 1 ) * sizeof(int*));
    for(int i=0; i<(*region).num_of_solutions + 1; i++) {
        new_solutions[i] = (int*)malloc(num_of_nodes * sizeof(int));
    }
    for(int i=0; i<(*region).num_of_solutions; i++) {
        memcpy(new_solutions[i], (*region).solutions[i], num_of_nodes*sizeof(int));
    }
    memcpy(new_solutions[(*region).num_of_solutions], solution, num_of_nodes * sizeof(int));
    for(int i=0; i<(*region).num_of_solutions; i++) {
        free((*region).solutions[i]);
    }
    free((*region).solutions);
    (*region).solutions = new_solutions;
    (*region).num_of_solutions++;
}
void move_solution(region **regions, int source_region, int destination_region, int solution_index, int num_of_nodes) {
    add_solution_to_region(&(*regions)[destination_region], (*regions)[source_region].solutions[solution_index], num_of_nodes);
    remove_solution_from_region(&(*regions)[source_region], solution_index, num_of_nodes);
}
void evolve_region(region* region, int num_of_nodes, adjacent_vertices_list** graph) {
    for(int i=0; i< (*region).num_of_solutions; i++) {
        two_opt_local_search(&(*region).solutions[i], num_of_nodes, graph);
    }
}