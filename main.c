#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "membrane.h"
#include "graph.h"
#include "tsp.h"

#define SEND_TAG 1764
#define RECEIVE_TAG 4859

void serialize_region(region region, int **region_buffer, int num_of_nodes) {
    (*region_buffer) = (int*)calloc((region.num_of_solutions * num_of_nodes), sizeof(int));
    for(int i=0; i<region.num_of_solutions; i++) {
        memcpy((*region_buffer) + num_of_nodes*i, region.solutions[i], num_of_nodes * sizeof(int));
    }
}
void deserialize_region(region *region, int *region_buffer, int num_of_solutions, int num_of_nodes) {
    (*region).num_of_solutions = num_of_solutions;
    (*region).solutions = (int**)malloc(num_of_solutions * sizeof(int*));
    for(int i=0; i<(*region).num_of_solutions; i++) {
        (*region).solutions[i] = (int*)malloc(num_of_nodes * sizeof(int));
        memcpy((*region).solutions[i], (region_buffer + num_of_nodes*i), num_of_nodes * sizeof(int));
    }
}

void replace_solution(region* region, int solution_id, int* new_solution, int num_of_nodes) {
    memcpy((*region).solutions[solution_id], new_solution, num_of_nodes * sizeof(int));
}

void distribute_work_on_regions(region* regions, int num_of_regions, int num_of_nodes) {

    int *region_buffer;
    for(int i=0; i<num_of_regions; i++) {
        serialize_region(regions[i], &region_buffer, num_of_nodes);
        MPI_Send(&regions[i].num_of_solutions, 1, MPI_INT, i + 1, SEND_TAG, MPI_COMM_WORLD);
        MPI_Send(region_buffer, num_of_nodes * regions[i].num_of_solutions, MPI_INT, i + 1, SEND_TAG, MPI_COMM_WORLD);
        free(region_buffer);
    }

}

void update_region(region** regions, int region_id, int num_of_regions, int num_of_nodes, int *updated_solutions, adjacent_vertices_list** graph) {

    for(int i=0; i<(*regions)[region_id].num_of_solutions; i++) {
        float initial_tour_cost = compute_cost_of_tour((*regions)[region_id].solutions[i], num_of_nodes, graph);
        float updated_tour_cost = compute_cost_of_tour(updated_solutions + i*num_of_nodes, num_of_nodes, graph);

        printf("Old solution with cost=%.2f:\n", compute_cost_of_tour((*regions)[region_id].solutions[i], num_of_nodes, graph));
        display_tour((*regions)[region_id].solutions[i], num_of_nodes);

        replace_solution(&((*regions)[region_id]), i, updated_solutions + i*num_of_nodes, num_of_nodes);

        printf("New solution with cost=%.2f:\n", compute_cost_of_tour((*regions)[region_id].solutions[i], num_of_nodes, graph));
        display_tour((*regions)[region_id].solutions[i], num_of_nodes);

        printf("\n");
        if(updated_tour_cost <= initial_tour_cost) {
            if(region_id > 0){
                printf("Moving solution #%d from region #%d in region #%d\n", i, region_id, region_id - 1);
                move_solution(regions, region_id, region_id - 1, i, num_of_nodes);
            }
        }
        else {
           if(region_id < (num_of_regions - 1)) {
               printf("Moving solution #%d from region #%d in region #%d\n", i, region_id, region_id + 1);
               move_solution(regions, region_id, region_id + 1, i, num_of_nodes);
           }
        }
    }
}
void receive_work_from_regions(region** regions, int num_of_regions, int num_of_nodes, adjacent_vertices_list** graph) {
    MPI_Status status;
    int *region_buffer;
    for(int i=0; i<num_of_regions; i++) {
        region_buffer = (int*)malloc((*regions)[i].num_of_solutions * num_of_nodes * sizeof(int));
        MPI_Recv(region_buffer, num_of_nodes * (*regions)[i].num_of_solutions, MPI_INT, i + 1, RECEIVE_TAG, MPI_COMM_WORLD, &status);

        printf("Region #%d:\n", i);

        update_region(regions, i, num_of_regions, num_of_nodes, region_buffer, graph);
        printf("\n\n\n\n");
    }
}
void distribution_step(region **regions, int num_of_regions, int num_of_nodes, adjacent_vertices_list** graph) {
    distribute_work_on_regions((*regions), num_of_regions, num_of_nodes);
    receive_work_from_regions(regions, num_of_regions, num_of_nodes, graph);
}

void evolution_step(int num_of_nodes, adjacent_vertices_list** graph) {
    int num_of_solutions;
    int *region_buffer;
    MPI_Status status;
    MPI_Recv( &num_of_solutions, 1, MPI_INT,0, SEND_TAG, MPI_COMM_WORLD, &status);
    region_buffer = (int*)malloc(num_of_solutions * num_of_nodes * sizeof(int));
    MPI_Recv(region_buffer, num_of_nodes*num_of_solutions, MPI_INT, 0, SEND_TAG, MPI_COMM_WORLD, &status);
    region region;
    deserialize_region(&region, region_buffer, num_of_solutions, num_of_nodes);
    evolve_region(&region, num_of_nodes, graph);
    free(region_buffer);
    serialize_region(region, &region_buffer, num_of_nodes);
    MPI_Send(region_buffer, num_of_nodes * num_of_solutions, MPI_INT, 0, RECEIVE_TAG, MPI_COMM_WORLD);
}
int main(int argc, char **argv) {
    int ierr, num_procs, my_id;


    int num_of_nodes = 5;
    adjacent_vertices_list** graph = (adjacent_vertices_list**)calloc(num_of_nodes, sizeof(adjacent_vertices_list*));
    build_graph_from_file("/Users/mihailplesa/Documents/Doctorat/Experiments/MPI2/tsp.txt", &graph);

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);


    if(my_id == 0) {
        srand(time(NULL));
        region *regions;
        int num_of_regions;
        initialize_regions_from_file(&regions, &num_of_regions, num_of_nodes, "/Users/mihailplesa/Documents/Doctorat/Experiments/MPI2/config.txt");
        printf("Root process:\n");
        display_regions(regions, num_of_regions, num_of_nodes, graph);

        distribution_step(&regions, num_of_regions, num_of_nodes, graph);
        printf("\n\n\n");
        printf("AFTER ONE ITERATION\n");
        display_regions(regions, num_of_regions, num_of_nodes, graph);

        distribution_step(&regions, num_of_regions, num_of_nodes, graph);
        printf("\n\n\n");
        printf("AFTER TWO ITERATION\n");
        display_regions(regions, num_of_regions, num_of_nodes, graph);

        distribution_step(&regions, num_of_regions, num_of_nodes, graph);
        printf("\n\n\n");
        printf("AFTER THREE ITERATION\n");
        display_regions(regions, num_of_regions, num_of_nodes, graph);

        distribution_step(&regions, num_of_regions, num_of_nodes, graph);
        printf("\n\n\n");
        printf("AFTER FOUR ITERATION\n");
        display_regions(regions, num_of_regions, num_of_nodes, graph);
    }
    else {
        evolution_step(num_of_nodes, graph);
        evolution_step(num_of_nodes, graph);
        evolution_step(num_of_nodes, graph);
        evolution_step(num_of_nodes, graph);
    }


    ierr = MPI_Finalize();
}