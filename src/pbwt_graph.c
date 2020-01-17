#include <stdio.h>
#include <string.h>
#include "pbwt.h"

/* Locally scoped function prototypes */
void sort_edges(edge_t *);
edge_t *sorted_merge(edge_t *, edge_t *);
edge_t *allocate_new_edge(const size_t, const double);


edge_t *allocate_new_edge(const size_t partner, const double w)
{
    edge_t *new_edge = NULL;
    new_edge = (edge_t *)malloc(sizeof(edge_t));
    if (new_edge == NULL)
    {
        return NULL;
    }
    new_edge->index = partner;
    new_edge->weight = w;
    new_edge->next = NULL;

    return new_edge;
}

adjlist_t *create_adjlist(const size_t V, char **samplist, char **reglist)
{
    size_t i = 0;
    adjlist_t *g = NULL;

    g = (adjlist_t *)malloc(sizeof(adjlist_t));
    if (g == NULL)
    {
        return NULL;
    }

    g->n_vertices = V;

    /* Create an array of adjacency lists.  Size of array will be V */
    g->nodelist = (vertex_t *)malloc(V * sizeof(vertex_t));
    if (g->nodelist == NULL)
    {
        return NULL;
    }

    /* Initialize each adjacency list as empty by making head as NULL */
    for (i = 0; i < V; ++i)
    {
    	g->nodelist[i].numconnect = 0;
    	g->nodelist[i].sampid = samplist[i];
    	g->nodelist[i].pop = reglist[i];
        g->nodelist[i].head = NULL;
    }

    return g;
}

void add_edge(adjlist_t *g, double w, size_t src, size_t dest)
{
    /* Add an edge from src to dest.  A new node is
    added to the adjacency list of src.  The node
    is added at the begining */
    edge_t *new_node = allocate_new_edge(dest, w);
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

void sort_edges(edge_t *h)
{
    size_t a = 0;
    double b = 0.0;
    edge_t *temp1 = NULL;
    edge_t *temp2 = NULL;

    for (temp1 = h; temp1 != NULL; temp1 = temp1->next)
    {
        for (temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next)
        {
            if (temp2->index < temp1->index)
            {
                a = temp1->index;
                b = temp1->weight;
                temp1->index = temp2->index;
                temp1->weight = temp2->weight;
                temp2->index = a;
                temp2->weight = b;
            }
        }
    }
}

edge_t *sorted_merge(edge_t *a, edge_t *b)
{
    edge_t *result = NULL;

    /* Base cases */
    if (a == NULL)
    {
        return b;
    }
    else if (b == NULL)
    {
        return a;
    }

    /* Pick either a or b, and recur */
    if (a->index < b->index)
    {
        result = a;
        result->next = sorted_merge(a->next, b);
    }
    else if (a->index == b->index)
    {
        result = a;
        result->weight = a->weight + b->weight;
        result->next = sorted_merge(a->next, b);
    }
    else
    {
        result = b;
        result->next = sorted_merge(a, b->next);
    }

    return result;
}

adjlist_t *diploidize(adjlist_t *g)
{
    size_t i = 0;
    size_t j = 0;
    size_t k = 0;
    size_t lj = 0;
    size_t nc = 0;
    size_t new_n_vertices = 0;
    adjlist_t *z = NULL;

    new_n_vertices = g->n_vertices / 2;
    z = (adjlist_t *)malloc(sizeof(adjlist_t));
    if (z == NULL)
    {
        return NULL;
    }
    z->n_vertices = new_n_vertices;
    z->nodelist = (vertex_t *)malloc(z->n_vertices * sizeof(vertex_t));
    if (z->nodelist == NULL)
    {
        return NULL;
    }

    for (i = 0; i < new_n_vertices; ++i)
    {
        j = 2 * i;
        k = 2 * i + 1;
        lj = strlen(g->nodelist[j].sampid);
        nc = 0;
        edge_t *r;
        edge_t *s;
        edge_t *f;
        edge_t *u;

        z->nodelist[i].sampid = (char *)malloc((lj - 1) * sizeof(char));
        if (z->nodelist[i].sampid == NULL)
        {
            return NULL;
        }
        strncpy(z->nodelist[i].sampid, g->nodelist[j].sampid, lj-2);
        z->nodelist[i].sampid[lj-2] = '\0';
        z->nodelist[i].pop = strdup(g->nodelist[j].pop);
        f = g->nodelist[j].head;
        sort_edges(f);
        s = g->nodelist[k].head;
        sort_edges(s);
        r = sorted_merge(f, s);
        z->nodelist[i].head = r;
        for (u = r, nc = 0; u != NULL; u = u->next)
        {
            u->index = u->index / 2;
            nc++;
        }
        z->nodelist[i].numconnect = nc;
    }

    return z;
}

void print_adjlist(const adjlist_t *g)
{
    size_t v = 0;

    for (v = 0; v < g->n_vertices; ++v)
    {
        if (g->nodelist[v].numconnect > 0)
        {
            edge_t *pcrawl = g->nodelist[v].head;
            while (pcrawl)
            {
                printf("%s\t%s\t%1.5lf\t%s\t%s\n", g->nodelist[v].sampid, g->nodelist[pcrawl->index].sampid,
                       pcrawl->weight, g->nodelist[v].pop, g->nodelist[pcrawl->index].pop);
                pcrawl = pcrawl->next;
            }
        }
    }
}
