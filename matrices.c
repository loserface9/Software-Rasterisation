#include "matrices.h"

#include <stdio.h>
#include <math.h>


void arraysub (double *w, const double *u, const double *v, const int length) {
    // Assigns w = u - v
    for (int i = 0; i < length; i++) {
        w[i] = u[i] - v[i];
    }
}


void Vec3_copy (Vec3 *restrict destination, const Vec3 *restrict origin) {
    (*destination)[0] = (*origin)[0];
    (*destination)[1] = (*origin)[1];
    (*destination)[2] = (*origin)[2];
}


void Mat2_inv (Mat2 *A) {
    // Invert a matrix
    const double denom =
        ((*A)[0][0] * (*A)[1][1]) -
        ((*A)[0][1] * (*A)[1][0]);

    const double A_00 = (*A)[0][0];
    (*A)[0][0] = (*A)[1][1] / denom;
    (*A)[0][1] = - (*A)[0][1] / denom;
    (*A)[1][0] = - (*A)[1][0] / denom;
    (*A)[1][1] = A_00 / denom;
}


int matmul(double *C, const double *A, const double *B, const int A_rows, const int A_cols, const int B_rows, const int B_cols) {
    const int C_rows = A_rows, C_cols = B_cols;
    if (A_cols != B_rows) {return -1;}

    int r, c, e; double element;
    for (r = 0; r < C_rows; r++) {
        for (c = 0; c < C_cols; c++) {
            element = 0;
            for (e = 0; e < A_cols; e++) {
                // Calculate the index: row * num_cols + column
                element = element + A[r*A_cols + e]*B[e*B_cols + c];
            }
            C[r*C_cols + c] = element;
        }
    }
    return 0;
}


void create_rotation_matrix_3d(Mat3 *rot_mat, const double theta_x, const double theta_y, const double theta_z) {
    Mat3 rot_mat_x = {
        {1, 0, 0},
        {0, cos(theta_x), -sin(theta_x)},
        {0, sin(theta_x), cos(theta_x)}
    };
    Mat3 rot_mat_y = {
        {cos(theta_y), 0, sin(theta_y)},
        {0, 1, 0},
        {-sin(theta_y), 0, cos(theta_y)}
    };
    Mat3 rot_mat_z = {
        {cos(theta_z), -sin(theta_z), 0},
        {sin(theta_z), cos(theta_z), 0},
        {0, 0, 1}
    };

    Mat3 rot_mat_zy;
    matmul((double *)&rot_mat_zy, (double *)&rot_mat_z, (double *)&rot_mat_y, 3, 3, 3, 3);
    matmul((double *)rot_mat, (double *)&rot_mat_zy, (double *)&rot_mat_x, 3, 3, 3, 3);
}


void Vec3_to_str (char str[], const Vec3 *P) {
    // str should be preallocated length 50*3
    sprintf(str, "[%lf, %lf, %lf]",
        *P[0], *P[1], *P[2]
    );
}


void print_array2d (double *A, int rows, int cols) {
    int r, c;
    printf("[");
    for (r = 0; r < rows; r++) {
        printf("[");
        for (c = 0; c < cols; c++) {
            printf("%f", A[r*cols + c]);
            if (c != cols-1) {printf(", ");}
            else {printf("]");}
        }
        if (r != rows-1) {printf(", ");}
        else {printf("]");}
    }
}
