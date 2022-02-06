//
//  auxiliar_functions.h
//  Flight_agenda
//
//  Created by Cibrán on 20/10/2021.
//

#ifndef main_functions_h
#define main_functions_h

#define min_day 1440 // Number of minutes in a day (1440 = 24x60).


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


typedef struct weighted_arrow {unsigned vertex_to; int departure_time; int fly_time;} weighted_arrow;
typedef struct node {char *name; unsigned short nsucc; weighted_arrow *arrow; unsigned distance; unsigned parent; unsigned departure;} node;
typedef struct queue_element_structure {unsigned v; struct queue_element_structure *seg;} queue_element;
typedef queue_element * priority_queue;


/* Here are all the needed functions defined */

void ExitError(const char *miss, int errcode) {
    fprintf(stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

/* Passes the information of the departure time to total minutes */
int departure_time_to_int(char *token) {
    int t_0 = (int) (token[0] - '0'), t_1 = (int) (token[1] - '0'), t_2 = (int) (token[2] - '0'), t_3 = (int) (token[3] - '0');
    return (t_0 * 10 + t_1) * 60 + (t_2 * 10 + t_3);
}

/* Passes the information of the fly time to total minutes */
int fly_time_to_int(char *token) {
    int t_0 = (int) (token[0] - '0'), t_1 = (int) (token[1] - '0'), t_2 = (int) (token[2] - '0'), t_3 = (int) (token[3] - '0'), t_4 = (int) (token[4] - '0');
    return (t_0 * 100 + t_1 * 10 + t_2) * 60 + (t_3 * 10 + t_4);
}

/* Extracts the number of days of total travel */
int extract_number_days(int distance) {
    int days = 0;
    
    while (distance >= min_day) {
        distance -= 1440;
        days += 1;
    }
    
    return days;
}

int node_search(char *idx_name, node nodes[], unsigned long N_nodes) {
    register unsigned i = 0;
    
    for (i = 0; i < N_nodes; i++)
        if (strcmp(idx_name, nodes[i].name) == 0)
            return i;
    
    return -1;
}

unsigned extract_min(priority_queue *Pq) {
    priority_queue first = *Pq; // It creates the structure 'first', with shape defined by 'priority_queue'.
    unsigned v = first -> v;
    *Pq = (*Pq) -> seg; // It's giving '*Pq' the value 'v' (located in '*Pq'). And the same above.
    free(first); // Deallocates the memory previously allocated by to first.
    return v;
}

void add_with_priority (unsigned v, priority_queue *Pq, node *nodes) {
    queue_element *aux = (queue_element *) malloc(sizeof(queue_element));
    
    if (aux == NULL)
        exit(66);
    
    aux -> v = v; // Gives to location 'v' in the struture 'aux' the value 'v'.
    float costv = nodes[v].distance;
    
    if (*Pq == NULL || !(costv > nodes[(*Pq) -> v].distance)) {
        aux -> seg = *Pq;
        *Pq = aux;
        return;
    }
    
    register queue_element * q;
    for (q = *Pq; q -> seg && nodes[q -> seg -> v].distance < costv; q = q -> seg);
    aux -> seg = q -> seg; // Gives 'seg' (in 'aux' structure) the value of 'seg' (in 'q').
    q -> seg = aux;
    return;
}

void decrease_priority (unsigned v, priority_queue *Pq, node *nodes) {
    if ((*Pq) -> v == v) // If 'v' in the queue it doesn't have to be re-ordered.
        return;
    
    float costv = nodes[v].distance;
    
    if (!(costv > nodes[(*Pq) -> v].distance)) {
        register queue_element *prepv; // Allocates it in a faster place than memory.
        for (prepv = *Pq; prepv -> seg -> v != v; prepv = prepv -> seg);
        queue_element * swap = *Pq;
        *Pq = prepv -> seg;
        prepv -> seg = prepv -> seg -> seg;
        (*Pq) -> seg = swap;
        return;
    }
    
    register queue_element *q, *prepv;
    for (q = *Pq; nodes[q -> seg -> v].distance < costv; q = q -> seg);
    
    if (q -> seg -> v == v)
        return;
    
    for (prepv = q -> seg; prepv -> seg -> v != v; prepv = prepv -> seg);
    queue_element *pv = prepv -> seg;
    prepv -> seg = pv -> seg;
    pv -> seg = q -> seg;
    q -> seg = pv;
    return;
}

/*
    Here the Dijkstra's algorithm is defined. It goes iteratively to the nearest neighbour, and if the distance trough the node is smaller than the other options, it saves it as a new edge of the sapanning tree. Once it finishes one brach, it goes through the previous non-expanded node.
*/
void Dijkstra (node *nodes, unsigned N_nodes, unsigned source, unsigned node_goal) {
    priority_queue Pq = NULL;
    char *expanded;
    int days;
    
    if ((expanded = (char*) malloc(N_nodes * sizeof(char))) == NULL)
        ExitError("when allocating memory for the expanded vector", 73);
    
    for (int i = 0; i < N_nodes; i++)
        expanded[i] = 0;
    
    add_with_priority(source, &Pq, nodes);
    
    while (Pq != NULL) {
        unsigned actual_node = extract_min(&Pq);
        expanded[actual_node] = 1;
        
        for (int i = 0; i < nodes[actual_node].nsucc; i++) {
            unsigned adj = nodes[actual_node].arrow[i].vertex_to;
            
            if (expanded[adj])
                continue;
            
            unsigned dist_aux;
            
            days = extract_number_days(nodes[actual_node].distance); // This makes possible to verify if we are able to take a flight.
            
            if (nodes[actual_node].distance <= (nodes[actual_node].arrow[i].departure_time + days * min_day))
                dist_aux = nodes[actual_node].arrow[i].departure_time + nodes[actual_node].arrow[i].fly_time + days * min_day;
            else // Not possible to take this flight.
                continue;
            
            if (nodes[adj].distance > dist_aux) {
                char Is_adj_in_Pq = nodes[adj].distance < UINT_MAX;
                
                nodes[adj].parent = actual_node;
                nodes[adj].departure = nodes[actual_node].arrow[i].departure_time + days * min_day;
                nodes[adj].distance = dist_aux;
                
                if (node_goal == adj)
                    break;
                
                if (Is_adj_in_Pq)
                    decrease_priority(adj, &Pq, nodes);
                else
                    add_with_priority(adj, &Pq, nodes);
            }
        }
    }
}

#endif /* main_functions_h */
