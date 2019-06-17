#ifndef CRIPTO_TP_MATRIX_H
#define CRIPTO_TP_MATRIX_H

long determinantOfMatrix(long **mat, int N, int n);

long **transpose(long **A, int n, int k);

long **multiply(long **mat1, long **mat2, int n, int m, int k);

long **inverse(long **m, int n, int inverses[251]);

long **add(long **mat1, long **mat2, int n);

long **subtract(long **mat1, long **mat2, int n);

void freeMatrix(long **m, int n);

//Kevin's

void printVector(int k, long* array, char* title);

//long* generateVector(int k, int initialValue);

//long** matX(int k, int n);

void printMatrix(int k, int n, long** matrix, char* title);

long** transposeV2(long** matrix, int n, int k); //TODO transpose MIGRATE TO THIS

long** multiplyV2(long **mat1, long **mat2, int n, int m, int k); //TODO multiply MIGRATE TO THIS

long** concat(long *vec, long **mat, int n, int k);

long** deconcatG(long **mat, int n, int k);

long* deconcatV(long **mat, int n);

#endif //CRIPTO_TP_MATRIX_H
