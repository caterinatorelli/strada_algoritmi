#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <math.h>

const int NODE_UNDEF = -1;

typedef struct
{
    int nord;
    int sud;
    int est;
    int ovest;
} Direction;

typedef struct Edge
{
    int src;       /* nodo sorgente */
    int dst;       /* nodo destinazione */
    double weight; /* peso dell'arco*/
    struct Edge *next;
} Edge;
typedef struct
{
    int n;        /* numero di nodi               */
    int m;        /* numero di archi              */
    Edge **edges; /* array di liste di adiacenza  */
    int *in_deg;  /* grado entrante dei nodi      */
    int *out_deg; /* grado uscente dei nodi       */
} Graph;
typedef struct
{
    int key;
    double prio;
} HeapElem;

typedef struct
{
    HeapElem *heap;
    int *pos; /* TODO: non inizializzato! Usare solo dopo una implementazione funzionante di minheap_change_prio() in cui la chiave da modificare si trova con una normale ricerca lineare sull'array heap[] */
    int n;    /* quante coppie (chiave, prio) sono effettivamente presenti nello heap */
    int size; /* massimo numero di coppie (chiave, prio) che possono essere contenuti nello heap */
} MinHeap;

MinHeap *Q;
/*fase 1*/
int *read_dimension(FILE *filein)
{
    int i = 0;

    int *dim = (int *)malloc(4 * sizeof(dim));
    for (i = 0; i < 4; i++)
    {
        fscanf(filein, "%d", &dim[i]);
    }
    return dim;
}

int **load_matrix(FILE *filein, int rows, int col)
{
    int i = 0, j = 0;
    int **matrix;

    matrix = (int **)calloc(rows, sizeof(*matrix));
    assert(matrix != NULL);

    for (i = 0; i < rows; i++)
    {

        matrix[i] = (int *)calloc(col, sizeof(**matrix));

        assert(matrix[i] != NULL);

        for (j = 0; j < col;)
        {
            fscanf(filein, "%d", &matrix[i][j]);
            j++;
        }
    }
    return matrix;
}
void matrix_destroy(int **matrix, int r)
{
    int i;
    for (i = 0; i < r; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}
Direction **init_costi(int row, int col)
{
    Direction **matrix;
    int i;
    matrix = (Direction **)calloc(row, sizeof(*matrix));
    assert(matrix != NULL);

    for (i = 0; i < row; i++)
    {

        matrix[i] = (Direction *)calloc(col, sizeof(**matrix));
    }
    return matrix;
}

/*
La costruzione della strada ha un costo che si compone di due parti:
1. Per ogni cella attraversata si sostiene un costo fisso Ccell intero non negativo.
2. Per ogni coppia di celle adiacenti attraversate, (xi, yi) e (xi  + 1, yi  + 1), si paga un ulteriore costo pari a
Cheight moltiplicato per il quadrato del dislivello, cioè Cheight × (H[xi, yi] – H[xi  + 1, yi  + 1])2. Tale costo è
zero nel caso in cui non ci sia dislivello tra (xi, yi) e (xi  + 1, yi  + 1). Cheight è un intero non negativo.
Quindi, se la strada attraversa le k celle (x0, y0), ... (xk-1, yk-1), il suo costo totale è dato da
Progettare e realizzare un algoritmo efficiente che determina le celle su cui costruire una strada di costo
complessivo minimo, rispettando i vincoli di cui sopra. Nel caso in cui esistano più strade di costo minimo è
sufficiente calcolarne una qualsiasi. Si noti che la soluzione ottima a questo problema non coincide
necessariamente con quella che attraversa il minor numero di celle.
Per evitare possibili problemi di overflow (che comunque non dovrebbero verificarsi con gli input che
verranno forniti), si usi il tipo long int oppure double per il costo totale della strada. Nel caso in cui si usi
il tipo double, si tenga presente che il risultato va stampato come intero, cioè senza la parte decimale (che
comunque deve essere sempre zero perché tutti i parametri del problema sono interi).
Input
Il programma deve accettare accetta un unico parametro sulla riga di comando, che rappresenta il nome di un
file di testo il cui contenuto rispetta la struttura seguente:
4
Ccell×k+Cheight×∑i=0
k−2
(H[xi,yi]−H[xi+1,yi+1])2

*/

/*dati i,j devo valutare i vicini della cella di matrix nelle posizioni nord,sud est,ovest nel caso queste posizioni siano impossibili restituisci INT_MAX*/
Direction calc_cost(int i, int j, int **matrix, int rows, int cols, int Ccell, int Cheight)
{
    Direction c;
    if (i == 0)
    {
        c.nord = INT_MAX;
    }
    else
    {
        c.nord = Ccell + Cheight * (matrix[i][j] - matrix[i - 1][j]) * (matrix[i][j] - matrix[i - 1][j]);
    }

    if (i == rows - 1)
    {
        c.sud = INT_MAX;
    }
    else
    {
        c.sud = Ccell + Cheight * (matrix[i][j] - matrix[i + 1][j]) * (matrix[i][j] - matrix[i + 1][j]);
    }

    if (j == 0)
    {
        c.ovest = INT_MAX;
    }
    else
    {
        c.ovest = Ccell + Cheight * (matrix[i][j] - matrix[i][j - 1]) * (matrix[i][j] - matrix[i][j - 1]);
    }

    if (j == cols - 1)
    {
        c.est = INT_MAX;
    }
    else
    {
        c.est = Ccell + Cheight * (matrix[i][j] - matrix[i][j + 1]) * (matrix[i][j] - matrix[i][j + 1]);
    }

    return c;
}
/*____________________________*/

/*GRAFO*/

int graph_n_nodes(const Graph *g)
{
    assert(g != NULL);

    return g->n;
}

Graph *graph_create(int n)
{
    int i;
    Graph *g = (Graph *)malloc(sizeof(*g));
    assert(g != NULL);
    assert(n > 0);

    g->n = n;
    g->m = 0;
    g->edges = (Edge **)malloc(n * sizeof(Edge *));
    assert(g->edges != NULL);
    g->in_deg = (int *)malloc(n * sizeof(*(g->in_deg)));
    assert(g->in_deg != NULL);
    g->out_deg = (int *)malloc(n * sizeof(*(g->out_deg)));
    assert(g->out_deg != NULL);
    for (i = 0; i < n; i++)
    {
        g->edges[i] = NULL;
        g->in_deg[i] = g->out_deg[i] = 0;
    }
    return g;
}
Edge *graph_adj(const Graph *g, int v)
{
    assert(g != NULL);
    assert((v >= 0) && (v < graph_n_nodes(g)));

    return g->edges[v];
}
static int exsit_edge(Graph *g, int src, int dst)
{
    Edge *e;
    for (e = graph_adj(g, src); e != NULL; e = e->next)
    {
        if (e->src == src && e->dst == dst)
        {
            return 1;
        }
    }
    return 0;
}
void graph_add_edge(Graph *g, int src, int dst, int weight)
{
    Edge *new_edge = (Edge *)malloc(sizeof(Edge));

    assert((src >= 0) && (src < graph_n_nodes(g)));
    assert((dst >= 0) && (dst < graph_n_nodes(g)));
    assert(new_edge != NULL);

    if (!exsit_edge(g, src, dst))
    {
        new_edge->src = src;
        new_edge->dst = dst;
        new_edge->weight = weight;
        new_edge->next = graph_adj(g, src);
        g->edges[src] = new_edge; 
        g->out_deg[src]++;
        g->in_deg[dst]++;
    }
    else
    {
        fprintf(stderr, "ERRORE: arco duplicato (%d,%d)\n", src, dst);
    }
}

int graph_id(int x, int y, int col)
{
    return x * col + y;
}
void fill_graph(Graph *g, Direction **costi, int n, int m)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            if (i > 0)
            {
                graph_add_edge(g, graph_id(i, j, m), graph_id(i - 1, j, m), costi[i][j].nord);
            }
            if (i < n - 1)
            {
                graph_add_edge(g, graph_id(i, j, m), graph_id(i + 1, j, m), costi[i][j].sud);
            }
            if (j > 0)
            {
                graph_add_edge(g, graph_id(i, j, m), graph_id(i, j - 1, m), costi[i][j].ovest);
            }
            if (j < m - 1)
            {
                graph_add_edge(g, graph_id(i, j, m), graph_id(i, j + 1, m), costi[i][j].est);
            }
        }
    }
}

int graph_index(int x, int y, int col)
{
    return x * col + y;
}

int matrix_i(int val, int col)
{
    return val / col;
}

int matrix_j(int val, int col)
{
    return val % col;
}

/*HEAP struttura di appoggio djkstra*/
void minheap_clear(MinHeap *h)
{
    int i;
    assert(h != NULL);
    for (i = 0; i < h->size; i++)
    {
        h->pos[i] = -1;
    }
    h->n = 0;
}
MinHeap *minheap_create(int size)
{
    MinHeap *h = (MinHeap *)malloc(sizeof(*h));
    assert(h != NULL);
    assert(size > 0);
    h->size = size;
    h->heap = (HeapElem *)malloc(size * sizeof(*(h->heap)));
    assert(h->heap != NULL);
    h->pos = (int *)malloc(size * sizeof(*(h->pos)));
    assert(h->pos != NULL);
    minheap_clear(h);
    return h;
}

void minheap_destroy(MinHeap *h)
{
    assert(h != NULL);

    h->n = h->size = 0;
    free(h->heap);
    free(h->pos);
    free(h);
}
static int valid(const MinHeap *h, int i)
{
    assert(h != NULL);

    return ((i >= 0) && (i < h->n));
}
static void swap(MinHeap *h, int i, int j)
{
    HeapElem tmp;

    assert(h != NULL);
    assert(valid(h, i));
    assert(valid(h, j));
    assert(h->pos[h->heap[i].key] == i);
    assert(h->pos[h->heap[j].key] == j);

    tmp = h->heap[i];
    h->heap[i] = h->heap[j];
    h->heap[j] = tmp;

    h->pos[h->heap[i].key] = i;
    h->pos[h->heap[j].key] = j;
}
static int parent(const MinHeap *h, int i)
{
    assert(valid(h, i));

    return (i + 1) / 2 - 1;
}

static int lchild(const MinHeap *h, int i)
{
    assert(valid(h, i));

    return 2 * i + 1;
}
static int rchild(const MinHeap *h, int i)
{
    assert(valid(h, i));

    return 2 * i + 2;
}
static int min_child(const MinHeap *h, int i)
{
    int l, r, result = -1;

    assert(valid(h, i));

    l = lchild(h, i);
    r = rchild(h, i);
    if (valid(h, l))
    {
        result = l;
        if (valid(h, r) && (h->heap[r].prio < h->heap[l].prio))
        {
            result = r;
        }
    }
    return result;
}
static void move_up(MinHeap *h, int i)
{
    int p;

    assert(valid(h, i));

    p = parent(h, i);
    while (valid(h, p) && (h->heap[i].prio < h->heap[p].prio))
    {
        swap(h, i, p);
        i = p;
        p = parent(h, i);
    }
}
static void move_down(MinHeap *h, int i)
{
    int done = 0;

    assert(valid(h, i));

    /* L'operazione viene implementata iterativamente, sebbene sia
       possibile una implementazione ricorsiva probabilmente più
       leggibile. */
    do
    {
        const int dst = min_child(h, i);
        if (valid(h, dst) && (h->heap[dst].prio < h->heap[i].prio))
        {
            swap(h, i, dst);
            i = dst;
        }
        else
        {
            done = 1;
        }
    } while (!done);
}
int minheap_is_empty(const MinHeap *h)
{
    assert(h != NULL);

    return (h->n == 0);
}

/* é esaurita la capienza a disposizione */
int minheap_is_full(const MinHeap *h)
{
    assert(h != NULL);

    return (h->n == h->size);
}
int minheap_min(const MinHeap *h)
{
    assert(!minheap_is_empty(h));

    return h->heap[0].key;
}
void minheap_insert(MinHeap *h, int key, double prio)
{
    int i;

    assert(!minheap_is_full(h));
    assert((key >= 0) && (key < h->size));
    assert(h->pos[key] == -1);

    i = h->n++;
    h->pos[key] = i;
    h->heap[i].key = key;
    h->heap[i].prio = prio;
    move_up(h, i);
}
int minheap_delete_min(MinHeap *h)
{
    int result;

    assert(!minheap_is_empty(h));

    result = minheap_min(h);
    swap(h, 0, h->n - 1);
    assert(h->heap[h->n - 1].key == result);
    h->pos[result] = -1;
    h->n--;
    if (!minheap_is_empty(h))
    {
        move_down(h, 0);
    }
    return result;
}
void minheap_change_prio(MinHeap *h, int key, double newprio)
{
    int j;
    double oldprio;

    assert(h != NULL);
    assert(key >= 0 && key < h->size);

    j = h->pos[key];
    assert(valid(h, j));
    oldprio = h->heap[j].prio;
    h->heap[j].prio = newprio;
    if (newprio > oldprio)
    {
        move_down(h, j);
    }
    else
    {
        move_up(h, j);
    }
}
/***
 * modifica dell'algoritmo di Dikstra per calcolare il percorso minimo
 * dove passiamo il valore di Ccell per calcolare il costo fisso per ogni cella attraversata
 * @param const Graph *g grafo
 * @param int s nodo sorgente
 * @param double *d array dei costi minimi
 * @param int *p array dei predecessori
 * @param const Edge **sp array degli archi del percorso minimo
 * 
 * @param int Ccell costo fisso per ogni cella attraversata 
 */

void dijkstra(const Graph *g, int s, double *d, int *p, const Edge **sp, int Ccell)
{
    int i;
    int v, u;
    const Edge *e;
    /*inizializzazione contenuti*/
    for (i = 0; i < g->n; i++)
    {
        sp[i] = NULL;
        d[i] = HUGE_VAL;
        p[i] = NODE_UNDEF;
    }
    /*in laboratorio viene dato il codice con d[s]=0
    ma qui lo modifico per avere l'algoritmo con costo costante
    come da specifiche della consegna*/
    d[s] = Ccell;
    p[s] = NODE_UNDEF;
    /*inizializzazione coda di priorità*/
    Q = minheap_create(g->n);

    for (i = 0; i < g->n; i++)
    {
        minheap_insert(Q, i, d[i]);
    }
    while (!minheap_is_empty(Q))
    {
        u = minheap_delete_min(Q);
        e = graph_adj(g, u);
        while (e != NULL)
        {
            v = e->dst;
            if (d[v] > d[u] + e->weight)
            {
                d[v] = d[u] + e->weight;
                p[v] = u;
                sp[v] = e;

                minheap_change_prio(Q, v, d[v]);
            }
            e = e->next;
        }
    }
}

int graph_out_degree(const Graph *g, int v)
{
    assert(g != NULL);
    assert((v >= 0) && (v < graph_n_nodes(g)));
    return g->out_deg[v];
}
void graph_print(const Graph *g)
{
    int i;

    assert(g != NULL);

    for (i = 0; i < g->n; i++)
    {
        const Edge *e;
        int out_deg = 0; /* ne approfittiamo per controllare la
                            correttezza dei gradi uscenti */
        printf("[%2d] -> ", i);
        for (e = graph_adj(g, i); e != NULL; e = e->next)
        {
            printf("(%d, %d, %f) -> ", e->src, e->dst, e->weight);
            out_deg++;
        }
        assert(out_deg == graph_out_degree(g, i));
        printf("NULL\n");
    }
}

int main(int argc, char const *argv[])
{
    int n, m;
    int Ccell;
    int Cheight;
    int *r_c;
    int **matrix;
    int i, j;
    double *d;
    int *prec;
    const Edge **s;
    Direction **costi;
    FILE *filein = stdin;
    Graph *g;
    int target;
    int *path;
    int index;

    if (argc != 2)
    {
        fprintf(stderr, "Invocare il programma con: %s matrice\n", argv[0]);
        return EXIT_FAILURE;
    }
    filein = fopen(argv[1], "r");
    if (filein == NULL)
    {
        fprintf(stderr, "Can not open %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    r_c = read_dimension(filein);
    Ccell = r_c[0];
    Cheight = r_c[1];
    n = r_c[2];
    m = r_c[3];
    matrix = load_matrix(filein, n, m);

    costi = init_costi(n, m);
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            costi[i][j] = calc_cost(i, j, matrix, n, m, Ccell, Cheight);
        }
    }
    fclose(filein);
    g = graph_create(n * m);
    fill_graph(g, costi, n, m);
    
    matrix_destroy(matrix, n);
    d = (double *)malloc(g->n * sizeof(*d));
    assert(d != NULL);
    prec = (int *)malloc(g->n * sizeof(*prec));
    assert(prec != NULL);
    s = (const Edge **)malloc(g->n * sizeof(*s));
    assert(s != NULL);
    /*graph_print(g);*/

    dijkstra(g, 0, d, prec, s,Ccell);
    target = graph_id(n - 1, m - 1, m); 
    if (d[target] == HUGE_VAL)
    {
        printf("No path found\n");
    }
    else
    {
        int path_length = 0;
        for (i = target; i != NODE_UNDEF; i = prec[i])
        {
            path_length++;
        }

        path = (int *)malloc(path_length * sizeof(int));
        assert(path != NULL);

        index = path_length - 1;
        for (i = target; i != NODE_UNDEF; i = prec[i])
        {
            path[index--] = i;
        }

        for (i = 0; i < path_length; i++)
        {
            printf("%d %d\n", matrix_i(path[i], m), matrix_j(path[i], m));
        }
        printf("-1 -1\n");
        printf("%ld\n", (long int)d[target]);

        free(path);
    }

    free(d);
    free(prec);
    free(s);
    

    return 0;
}
