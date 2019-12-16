#include <stdio.h>
#include <string.h>
#include "pbwt.h"

edge* allocate_new_edge(const size_t partner, const double w)
{
    edge* new_edge = (edge*)malloc(sizeof(edge));
    new_edge->index = partner;
    new_edge->weight = w;
    new_edge->next = NULL;
    return new_edge;
}

adjlist* create_adjlist(const size_t V, char **samplist, char **reglist)
{
    adjlist *g = (adjlist*)malloc(sizeof(adjlist));
    g->n_vertices = V;

    /* Create an array of adjacency lists.  Size of array will be V */
    g->nodelist = (vertex*)malloc(V * sizeof(vertex));

    /* Initialize each adjacency list as empty by making head as NULL */
    size_t i = 0;
    for (i = 0; i < V; ++i)
    {
    	g->nodelist[i].numconnect = 0;
    	g->nodelist[i].sampid = samplist[i];
    	g->nodelist[i].pop = reglist[i];
        g->nodelist[i].head = NULL;
    }

    return g;
}

void add_edge(adjlist *g, double w, size_t src, size_t dest)
{
    /* Add an edge from src to dest.  A new node is
    added to the adjacency list of src.  The node
    is added at the begining */
    edge* new_node = allocate_new_edge(dest, w);
    new_node->next = g->nodelist[src].head;
    g->nodelist[src].head = new_node;
    g->nodelist[src].numconnect++;

    /* Since graph is undirected, add an edge from
    dest to src also */
    new_node = allocate_new_edge(src, w);
    new_node->next = g->nodelist[dest].head;
    g->nodelist[dest].head = new_node;
    g->nodelist[dest].numconnect++;
}

void print_adjlist(adjlist *g)
{
    size_t v = 0;

    for (v = 0; v < g->n_vertices; ++v)
    {
        if (g->nodelist[v].numconnect > 0)
        {
            edge *pcrawl = g->nodelist[v].head;
            while (pcrawl)
            {
                printf("%s\t%s\t%1.5lf\t%s\t%s\n", g->nodelist[v].sampid, g->nodelist[pcrawl->index].sampid,
                       pcrawl->weight, g->nodelist[v].pop, g->nodelist[pcrawl->index].pop);
                pcrawl = pcrawl->next;
            }
        }
    }
}
