//
//  flight_agenda.c
//  Flight_agenda
//
//  Created by Cibrán on 1/10/2021.
//


#include "auxiliar_functions.h"

#define input_file_name "Flight201301u.csv"
#define node_star_time 9*60


/* Here starts the main program, executing the previously defined Dijikstra's algorithm. Different combinations of starting and ending airports can be selected. */

int main() {
    clock_t start;
    start = clock();
    char *token, *data_string = NULL;
    long new_position, actual_position;
    unsigned int i, N_comments = 1U, N_nodes = 0U;
    
    size_t buffer_size = 0;
    
    /* First we load the data from the files */
    FILE *flights_data;
    if ((flights_data = fopen(input_file_name, "r")) == NULL)
        ExitError("The input data file cannot be opened", 1);
    
    /* Opening the file */
    if (flights_data == NULL)
        ExitError("\nWe cannot acces to the nodes data file\n", 2);
    
    /* Reserving memory for the nodes */
    node *nodes;
    if ((nodes = (node *) malloc(sizeof(node))) == NULL)
        ExitError("Error in the memory allocation of nodes\n", 3);
    
    
    /* Skipping comments */
    for (i = 0; i < N_comments; i++)
        getline(&data_string, &buffer_size, flights_data);
    
    /* Loading the data */
    printf("--- Loading the data ---\n");
    while (getline(&data_string, &buffer_size, flights_data) != EOF) {
        token = strsep(&data_string, ";");
                
        /* Tryes to initialize a new airport */
        if ((actual_position = node_search(token, nodes, N_nodes)) < 0) {
            actual_position = N_nodes;
            N_nodes++;
            
            if ((nodes = realloc(nodes, (N_nodes) * sizeof(node))) == NULL)
                ExitError("Error in the memory allocation of nodes\n", 6);
            
            asprintf(&nodes[actual_position].name, "%s", token);
            nodes[actual_position].nsucc = 0;
            
            if ((nodes[actual_position].arrow = (weighted_arrow *) malloc(sizeof(weighted_arrow))) == NULL)
                ExitError("Error in the memory allocation of successors\n", 5);
        }
        
        /* If it is already initialized */
        else
            if ((nodes[actual_position].arrow = realloc(nodes[actual_position].arrow, (nodes[actual_position].nsucc+1) * sizeof(weighted_arrow))) == NULL)
                ExitError("Error in the memory reallocation of successors\n", 6);
        
        token = strsep(&data_string, ";");
                
        /* Looks for the successor. If it does not exist, we initialize it as node as well */
        if ((new_position = node_search(token, nodes, N_nodes)) < 0) {
            new_position = N_nodes;
            N_nodes++; // One new node.
            
            if ((nodes = realloc(nodes, (N_nodes) * sizeof(node))) == NULL)
                ExitError("Error in the memory allocation of nodes\n", 6);
            
            asprintf(&nodes[new_position].name, "%s", token);
            nodes[new_position].nsucc = 0;
            
            if ((nodes[new_position].arrow = (weighted_arrow *) malloc(sizeof(weighted_arrow))) == NULL)
                ExitError("Error in the memory allocation of successors\n", 5);
        }
        
        nodes[actual_position].arrow[nodes[actual_position].nsucc].vertex_to = new_position;
        nodes[actual_position].nsucc++;
        
        token = strsep(&data_string, ";");
        nodes[actual_position].arrow[nodes[actual_position].nsucc-1].departure_time = departure_time_to_int(token);
        
        /* Skipps the non-usefull data */
        for (i = 0; i < 3; i++)
            token = strsep(&data_string, ";");
        
        nodes[actual_position].arrow[nodes[actual_position].nsucc-1].fly_time = fly_time_to_int(token);
    } fclose(flights_data);
    
    for (i = 0; i < N_nodes; i++) {
        nodes[i].distance = UINT_MAX;
        nodes[i].parent = UINT_MAX;
    }
    
    //char *start_name = "REU", *goal_name = "ILD";
    //char *start_name = "AAA", *goal_name = "GKA";
    char *start_name = "GDT", *goal_name = "JGO";
    unsigned node_start = node_search(start_name, nodes, N_nodes), node_goal = node_search(goal_name, nodes, N_nodes);
    
    /* We only want the departure from 9am to be taken into account. */
    nodes[node_start].distance = node_star_time;
    
    Dijkstra(nodes, N_nodes, node_start, node_goal);
    
    register unsigned v = node_goal, pv = nodes[v].parent, ppv;
    
    while (v != node_start) { // Reconstructs the path backwards.
        ppv = nodes[pv].parent;
        nodes[pv].parent = v;
        v = pv;
        pv = ppv;
    }
    
    nodes[node_goal].parent = UINT_MAX;
    
    printf("Optimal path found:\n");
    printf("  Airport name  | Departure | Arrival\n");
    printf("  ------------  | --------- | -------\n");
    printf("       %s      |           |   %u\n", nodes[node_start].name, nodes[node_start].distance);

    for (v = nodes[node_start].parent; v != UINT_MAX; v = nodes[v].parent)
        printf("       %s      |    %u   |   %u\n", nodes[v].name, nodes[v].departure, nodes[v].distance);
    
    printf("--- Finished in %.2f seconds ---\n", (float) (clock() - start) / CLOCKS_PER_SEC);
}
