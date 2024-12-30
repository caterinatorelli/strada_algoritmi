#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

    /*inizializzo la matrice con calloc in maniera da pulire le memeorie*/
    matrix = (int **)calloc(rows, sizeof(*matrix));
    assert(matrix != NULL);
    printf("%d ", matrix[i][j]);

    for (i = 0; i < rows; i++)
    {

        matrix[i] = (int *)calloc(col, sizeof(**matrix));

        assert(matrix[i] != NULL);
        /*avanza con il riempimento della riga solo quando trova numeri consecutivi*/
        for (j = 0; j < col;)
        {
            fscanf(filein, "%d", &matrix[i][j]);
            printf("%d ", matrix[i][j]);
            j++;
        }
    }
    return matrix;
}

int main(int argc, char const *argv[])
{
    int rows, cols;
    int Ccell;
    int Cheight;
    int *r_c;
    int **matrix;

    FILE *filein = stdin;
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
    rows = r_c[2];
    cols = r_c[3];
    matrix = load_matrix(filein, rows, cols);
}
