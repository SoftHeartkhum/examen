#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define FILAS 3
#define COLUMNAS 3
#define NNZ 3 // Número de elementos no nulos

void multiplicar_filas(int *puntero_filas, int *indices_columnas, double *valores, double *x, double *resultado_parcial, int fila_inicio, int fila_fin) {
    for (int i = fila_inicio; i < fila_fin; i++) {
        resultado_parcial[i - fila_inicio] = 0.0;
        for (int j = puntero_filas[i]; j < puntero_filas[i + 1]; j++) {
            resultado_parcial[i - fila_inicio] += valores[j] * x[indices_columnas[j]];
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rango, tamano;
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamano);

    // Definir la matriz dispersa A en formato CSR (hardcoded para simplicidad)
    int puntero_filas[FILAS + 1] = {0, 1, 2, 3};
    int indices_columnas[NNZ] = {0, 1, 2};
    double valores[NNZ] = {1.0, 2.0, 3.0};
    double x[COLUMNAS] = {4.0, 5.0, 6.0};

    int filas_por_proceso = (FILAS + tamano - 1) / tamano;
    int fila_inicio = rango * filas_por_proceso;
    int fila_fin = (rango + 1) * filas_por_proceso;
    if (fila_fin > FILAS) fila_fin = FILAS;

    double resultado_parcial[filas_por_proceso];
    multiplicar_filas(puntero_filas, indices_columnas, valores, x, resultado_parcial, fila_inicio, fila_fin);

    double resultado[FILAS];
    MPI_Gather(resultado_parcial, filas_por_proceso, MPI_DOUBLE, resultado, filas_por_proceso, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rango == 0) {
        printf("Vector resultado:\n");
        for (int i = 0; i < FILAS; i++) {
            printf("%f\n", resultado[i]);
        }
    }

    MPI_Finalize();
    return 0;
}
