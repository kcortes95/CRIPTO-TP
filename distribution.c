#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "random.h"
#include "distribution.h"
#include "matrix.h"
#include "modular.h"

// rank k
// det != 0
// proj(A) y S - proj(A) no debe tener valores mayores que 251
int **matA(int n, int k) {
    int **A = (int **) malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) A[i] = (int *) malloc(k * sizeof(int));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            A[i][j] = (int) (urandom() % 251); // 251 because A must have values in [0, 251)
        }
    }

    // Paper case
    A[0][0] = 3;
    A[0][1] = 7;
    A[1][0] = 6;
    A[1][1] = 1;
    A[2][0] = 2;
    A[2][1] = 5;
    A[3][0] = 6;
    A[3][1] = 6;

    printf("A matrix:\n");
    for (int row = 0; row < n; row++) {
        for (int columns = 0; columns < k; columns++) {
            printf("  %d", A[row][columns]);
        }
        printf("\n");
    }

    return A;
}

int **projectionSd(int **A, int n, int k, int inverses[251]) {
    int **At = (int **) malloc(k * sizeof(int *));
    for (int i = 0; i < k; i++) At[i] = (int *) calloc((size_t) n, sizeof(int));

    transpose(A, At, n, k);

    int **AtA = (int **) malloc(k * sizeof(int *));
    for (int i = 0; i < k; i++) AtA[i] = (int *) calloc((size_t) k, sizeof(int));

    printf("\n");

    printf("At matrix:\n");
    for (int row = 0; row < k; row++) {
        for (int columns = 0; columns < n; columns++) {
            printf("  %d", At[row][columns]);
        }
        printf("\n");
    }

    multiply(At, A, AtA, k, k, n);

    printf("\n");
    printf("pre AtA matrix:\n");
    for (int row = 0; row < k; row++) {
        for (int columns = 0; columns < k; columns++) {
            printf("  %d", AtA[row][columns]);
        }
        printf("\n");
    }

    int det = determinantOfMatrix(AtA, k, k);

    printf("post AtA matrix:\n");
    for (int row = 0; row < k; row++) {
        for (int columns = 0; columns < k; columns++) {
            printf("  %d", AtA[row][columns]);
        }
        printf("\n");
    }

    printf("\n");
    printf("det: %d\n", det);

    if (det != 0) {
        printf("%s", "Inverse exists!\n");

        int **AugmentedAtaInverse = (int **) malloc(k * sizeof(int *)); //TODO free
        for (int i = 0; i < k; i++) AugmentedAtaInverse[i] = (int *) calloc((size_t) k * 2, sizeof(int));

        inverse(AtA, AugmentedAtaInverse, k, inverses);

        freeMatrix(AtA, k);

        printf("\n");
        printf("AugmentedAtaInverse matrix:\n");
        for (int row = 0; row < k; row++) {
            for (int columns = 0; columns < k * 2; columns++) {
                printf("  %d", AugmentedAtaInverse[row][columns]);
            }
            printf("\n");
        }

        int **AtAInverse = (int **) malloc(k * sizeof(int *));
        for (int i = 0; i < k; i++) AtAInverse[i] = (int *) calloc((size_t) k, sizeof(int));

        // Fill AtAInverse
        for (int row = 0; row < k; row++) {
            for (int columns = 0; columns < k; columns++) {
                AtAInverse[row][columns] = AugmentedAtaInverse[row][columns + k];
            }
        }

        printf("\n");
        printf("AtAInverse matrix:\n");
        for (int row = 0; row < k; row++) {
            for (int columns = 0; columns < k; columns++) {
                printf("  %d", AtAInverse[row][columns]);
            }
            printf("\n");
        }

        freeMatrix(AugmentedAtaInverse, k);

        // A * (At * A)'
        int **AxInverse = (int **) malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) AxInverse[i] = (int *) calloc(1, k * sizeof(int));

        multiply(A, AtAInverse, AxInverse, n, k, k);

        freeMatrix(AtAInverse, k);

        printf("\n");
        printf("AxInverse matrix:\n");
        for (int row = 0; row < n; row++) {
            for (int columns = 0; columns < k; columns++) {
                printf("  %d", AxInverse[row][columns]);
            }
            printf("\n");
        }

        // (A * (At * A)') * At
        int **proj = (int **) malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) proj[i] = (int *) calloc(1, k * sizeof(int));

        multiply(AxInverse, At, proj, n, n, k);

        freeMatrix(AxInverse, n);
        freeMatrix(At, k);

        return proj;
    } else {
        printf("%s", "Inverse does not exist!");

        return NULL;
    }
}

int **remainderR(int **secretS, int **projectionSd, int n) {
    int **difference = (int **) malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) difference[i] = (int *) calloc((size_t) n, sizeof(int));

    subtract(secretS, projectionSd, difference, n);

    return difference;
}

int **remainderRw(int **watermarkW, int **projectionSd, int n) {
    int **difference = (int **) malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) difference[i] = (int *) calloc((size_t) n, sizeof(int));

    subtract(watermarkW, projectionSd, difference, n);

    return difference;
}

/*
 * R is the remainder matrix.
 * c is the coefficents array (from 1 to n).
 * n is the max number of participants.
 * k is the min number of participants.
 * t is the current participant index.
 */
int *g_i_j(int **R, int initial_column, int t, int n, int k) {
    int *res = (int *) calloc(n, sizeof(int));

    for (int row = 0; row < n; row++) {
        for (int column = initial_column; column < k + initial_column; column++) {
            res[row] += R[row][column] * pow(t, column - initial_column);
        }
        res[row] = modulo(res[row], 251);
    }

    return res;
}

/*
 * R is the remainder matrix.
 * n is the max number of participants.
 * k is the min number of participants.
 * t is the current participant index.
 */
int **matG_t(int **R, int n, int k, int t) {
    int max_t = (int) ceil(n / k); // TODO: code method for create matrix !
    int **res = (int **) calloc(max_t, sizeof(int *));

    for (int i = 0; i < max_t; i++) {
        res[i] = g_i_j(R, i * 2, t, n, k);
    }

    int **resT = (int **) malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) resT[i] = (int *) calloc(max_t, sizeof(int));
    transpose(res, resT, max_t, n);

    freeMatrix(res, max_t);

    return resT;
}

/*
 * R is the remainder matrix.
 * n is the max number of participants.
 * k is the min number of participants.
 */
int ***matG(int **R, int n, int k) {
    int ***matG = (int ***) malloc(n * sizeof(int **));

    for (int t = 0; t < n; t++) {
        matG[t] = matG_t(R, n, k, t + 1);
    }

    return matG;
}