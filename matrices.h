#ifndef MATRICES_H
#define MATRICES_H

typedef double Vec2[2];
typedef double Vec3[3];
typedef double Mat2[2][2];
typedef double Mat3[3][3];

void Vec3_to_str (char str[], const Vec3 *P);
void Vec3_copy (Vec3 *destination, const Vec3 *origin);
void print_array2d(double *A, int rows, int cols);
void arraysub(double *w, const double *u, const double *v, const int length);
void Mat2_inv(Mat2 *A);
int matmul(double *C, const double *A, const double *B, const int A_rows, const int A_cols, const int B_rows, const int B_cols);
void create_rotation_matrix_3d(Mat3 *rot_mat, const double theta_x, const double theta_y, const double theta_z);

#endif
