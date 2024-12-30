#include <stdio.h>


typedef struct {
    int x, y;
    long costo;
} Cell;

int cellscomparator (void *a,void *b) { 
    Cell *cell1 = (Cell *) a;
    Cell *cell2 = (Cell *) b;
     if ((*cell1).costo < (*cell2).costo) {
        return -1; // Il costo di cell1 è minore, cell1 viene prima
    } else if ((*cell1).costo > (*cell2).costo) {
        return 1;  // Il costo di cell1 è maggiore, cell1 viene dopo
    } else {
        return 0;  // I costi sono uguali
    }

}

int costCalculator (int Ccell, int Cheight, int height1, int height2)
{
    int diff=height1-height2;
    int cost=Ccell + Cheight *(diff * diff);
    return cost;
}

int main() {
    FILE *file;
    int Ccell, Cheight, n, m, i, j;
    int **H, **cost, **predecessor;
    
    if (argc != 2) {
        printf("Uso: %s nome_file_input\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Errore nell'aprire il file\n");
        return 1;
    }

    // Lettura dei parametri dal file
    fscanf(file, "%d", &Ccell);
    fscanf(file, "%d", &Cheight);
    fscanf(file, "%d", &n);
    fscanf(file, "%d", &m);

    // Creazione della matrice delle altezze
    H = (int **) malloc(n * sizeof(int *));
    for (i = 0; i < n; i++) {
        H[i] = (int *) malloc(m * sizeof(int));
        for (j = 0; j < m; j++) {
            fscanf(file, "%d", &H[i][j]);
        }
    }
    fclose(file);

    int direction[4][2]={{0,1}, {0,-1}, {-1,0}, {1,0}} ; // destra sinistra su giu    
    
    Cell *heap = (Cell *) malloc(n * m * sizeof(Cell));
    int heapSize = 0;

    // Inizializza la cella di partenza
    cost[0][0] = 0;
    heap[heapSize++] = (Cell) {0, 0, 0};

    while (heapSize > 0) {
        qsort(heap, heapSize, sizeof(Cell), cellscomparator);
        Cell current = heap[heapSize--];
        int x =current.x ;
        int y= current.y;

        for (int d = 0; d < 4; d++) {
            int nx = x + direction[d][0];
            int ny = y + direction[d][1];

            if (nx < n && nx >= 0 && ny >= 0 && ny < m)
            {
               int costoCella=cost [x][y];
               int costoMovimento= costCalculator(Ccell,Cheight,H[x][y], H[nx][ny]);
               int costotot=costoCella + costoMovimento ;
               if (costotot < cost[nx][ny]) {
        // Aggiorno il costo della cella (nx, ny) con il nuovo costo calcolato
        cost[nx][ny] = costotot;

        predecessor[nx][ny] = x * m + y;
               }
        
            }
        }
    }
          printf("Costo minimo: %d\n", cost[n-1][m-1]);

    // Pulizia della memoria
    for (i = 0; i < n; i++) {
        free(H[i]);
        free(cost[i]);
        free(predecessor[i]);
    }
    free(H);
    free(cost);
    free(predecessor);
    free(heap);

    return 0;


}