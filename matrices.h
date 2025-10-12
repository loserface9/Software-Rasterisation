#ifndef MATRICES_H
#define MATRICES_H

void array3_to_str(char str[], double P[3]);
void print_array2d(double *A, int rows, int cols);
void arraysub(double *w, const double *u, const double *v, const int length);
void matinv2(double A[2][2]);
int matmul(double *C, double *A, double*B, int A_rows, int A_cols, int B_rows, int B_cols);
int create_rotation_matrix_3d(double rot_mat[3][3], double theta_x, double theta_y, double theta_z);

#endif
