#include "matrices.h"
#include "make_frame.h"
#include "bitmap.h"
#include "constants.h"

#include <SDL3/SDL_stdinc.h>
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


struct Triangle {
    double A[3], B[3], C[3];
    int color;
};


void Triangle_to_str(char str[], struct Triangle *triangle);
void transform_triangle(struct Triangle *restrict triangle, double trans_mat[3][3]);
void rasterise (
    int *restrict buffer, double *restrict z_buffer,
    const struct Triangle *restrict triangle,
    const double x_positions[WIN_WIDTH], const double y_positions[WIN_HEIGHT]
);
void clear_buffers (int *restrict buffer, double *restrict z_buffer);


int main () {
    struct Triangle t_1 = {
        {-1, -1, -1},
        {1, -1, -1},
        {1, 1, -1},
        2
    };
    struct Triangle t_2 = {
        {-1, -1, -1},
        {-1, 1, -1},
        {1, 1, -1},
        2
    };
    struct Triangle t_3 = {
        {-1, -1, -1},
        {-1, 1, -1},
        {-1, 1, 1},
        3
    };
    struct Triangle t_4 = {
        {-1, -1, -1},
        {-1, -1, 1},
        {-1, 1, 1},
        3
    };
    struct Triangle t_5 = {
        {-1, -1, -1},
        {-1, -1, 1},
        {1, -1, 1},
        4
    };
    struct Triangle t_6 = {
        {-1, -1, -1},
        {1, -1, -1},
        {1, -1, 1},
        4
    };
    struct Triangle t_7 = {
        {1, 1, -1},
        {1, -1, -1},
        {1, -1, 1},
        5
    };
    struct Triangle t_8 = {
        {1, 1, -1},
        {1, 1, 1},
        {1, -1, 1},
        5
    };
    struct Triangle t_9 = {
        {1, 1, -1},
        {1, 1, 1},
        {-1, 1, 1},
        6
    };
    struct Triangle t_10 = {
        {1, 1, -1},
        {-1, 1, -1},
        {-1, 1, 1},
        6
    };
    struct Triangle t_11 = {
        {-1, -1, 1},
        {1, 1, 1},
        {-1, 1, 1},
        7
    };
    struct Triangle t_12 = {
        {-1, -1, 1},
        {1, 1, 1},
        {1, -1, 1},
        7
    };

    struct Triangle *triangles[12] = {&t_1, &t_2, &t_3, &t_4, &t_5, &t_6, &t_7, &t_8, &t_9, &t_10, &t_11, &t_12};

    double x_positions[WIN_WIDTH];
    for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
        x_positions[xIdx] = ( ((double) xIdx/(WIN_WIDTH-1)) * (x_range*2) ) - x_range;
    }
    double y_positions[WIN_HEIGHT];
    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        y_positions[yIdx] = ( ((double) yIdx/(WIN_HEIGHT-1)) * (y_range*2) ) - y_range;
        y_positions[yIdx] = -y_positions[yIdx];
    }

    int *buffer = malloc(PIXEL_COUNT*sizeof(int));
    double *z_buffer = malloc(PIXEL_COUNT*sizeof(double));
    clear_buffers(buffer, z_buffer);

    double rot_mat[3][3];
    // create_rotation_matrix_3d(rot_mat, -M_PI/(12*16), M_PI/(12*8), 0);
    create_rotation_matrix_3d(rot_mat, -M_PI/(6), M_PI/(3), 0);

    for (int i = 0; i < 12; i++) {
        transform_triangle(triangles[i], rot_mat);
    }

    for (int i = 0; i < 12; i++) {
        rasterise(buffer, z_buffer, triangles[i], x_positions, y_positions);
    }

    // struct timespec wait_time = {0, 16*1000000};
    // int i;
    // while (false) {
    //     clear_buffers(buffer, z_buffer);
    //     for (i = 0; i < 12; i++) {
    //         transform_triangle(triangles[i], rot_mat);
    //     }
    //     for (i = 0; i < 12; i++) {
    //         rasterise(buffer, z_buffer, triangles[i], x_positions, y_positions);
    //     }

    //     nanosleep(&wait_time, NULL);
    //     system("clear");
    //     output_buffer_as_ascii(buffer);
    // }

    unsigned char *frame = calloc(PIXEL_COUNT*3, sizeof(unsigned char));
    make_frame(frame, buffer);

    const unsigned long FILE_SIZE = PIXEL_COUNT*4 + 14 + 40;
    const unsigned long FILE_SIZE_BYTES = FILE_SIZE*sizeof(unsigned char);
    unsigned char *bitmap = calloc(FILE_SIZE, sizeof(unsigned char));
    make_bitmap(bitmap, frame, WIN_HEIGHT, WIN_WIDTH);

    char filename[] = "bitmap.bmp";
    FILE *fp = fopen(filename, "w");
    fwrite(bitmap, FILE_SIZE_BYTES, 1, fp);
    fclose(fp);

    free(frame);
    free(bitmap);

    return 0;
}


void clear_buffers (int *restrict buffer, double *restrict z_buffer) {
// *buffer    - int [WIN_HEIGHT][WIN_WIDTH]
// *z_buffer  - double [WIN_HEIGHT][WIN_WIDTH]

    int yIdx, xIdx;
    for (yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        for (xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            buffer[yIdx*WIN_WIDTH + xIdx] = 0;
            z_buffer[yIdx*WIN_WIDTH + xIdx] = -DBL_MAX;
        }
    }
}


void rasterise (
    int *restrict buffer, double *restrict z_buffer,
    const struct Triangle *restrict triangle,
    const double x_positions[WIN_WIDTH], const double y_positions[WIN_HEIGHT]
) {
// *buffer    - int [WIN_HEIGHT][WIN_WIDTH]
// *z_buffer  - double [WIN_HEIGHT][WIN_WIDTH]

    double AB[3], AC[3];
    arraysub(AB, triangle->B, triangle->A, 3);
    arraysub(AC, triangle->C, triangle->A, 3);
    double n[3] = {
        AB[1]*AC[2] - AB[2]*AC[1],
        AB[2]*AC[0] - AB[0]*AC[2],
        AB[0]*AC[1] - AB[1]*AC[0]
    };
    // z = (n[0](A[0] - x) + n[1](A[1] - y))/n[2] + A[2]

    double ABAC_inv[2][2] = {
        {AB[0], AC[0]},
        {AB[1], AC[1]}
    };
    matinv2(ABAC_inv);

    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            double P[2] = {x_positions[xIdx], y_positions[yIdx]};
            double AP[2] = {P[0] - triangle->A[0], P[1] - triangle->A[1]};

            double bary[2];
            matmul(bary, (double *)ABAC_inv, AP, 2, 2, 2, 1);

            bool in_triangle = (
                ((1 > bary[0]) && (0 < bary[0])) &&
                ((1 > bary[1]) && (0 < bary[1])) &&
                ((bary[0] + bary[1]) < 1)
            );

            if (in_triangle) {
                double P_z = (n[0]*(triangle->A[0] - P[0]) + n[1]*(triangle->A[1] - P[1])) / n[2] + triangle->A[2];
                if (P_z > z_buffer[yIdx*WIN_WIDTH + xIdx]){
                    buffer[yIdx*WIN_WIDTH + xIdx] = triangle->color;
                    z_buffer[yIdx*WIN_WIDTH + xIdx] = P_z;
                }
            }
        }
    }
}


void transform_triangle(struct Triangle *restrict triangle, double trans_mat[3][3]) {
    double triangle_mat_prev[3][3] = {
        {triangle->A[0], triangle->B[0], triangle->C[0]},
        {triangle->A[1], triangle->B[1], triangle->C[1]},
        {triangle->A[2], triangle->B[2], triangle->C[2]}
    };
    double triangle_mat_curr[3][3];
    matmul((double *)triangle_mat_curr, (double *)trans_mat, (double *)triangle_mat_prev, 3, 3, 3, 3);
    *triangle = (struct Triangle) {
        {triangle_mat_curr[0][0], triangle_mat_curr[1][0], triangle_mat_curr[2][0]},
        {triangle_mat_curr[0][1], triangle_mat_curr[1][1], triangle_mat_curr[2][1]},
        {triangle_mat_curr[0][2], triangle_mat_curr[1][2], triangle_mat_curr[2][2]},
        triangle->color
    };
}


void Triangle_to_str (char str[], struct Triangle *triangle) {
    // str should be preallocated length 50*3*3
    array3_to_str(str, triangle->A);
    array3_to_str(&str[50*3], triangle->B);
    array3_to_str(&str[50*3*2], triangle->C);

    char str_1[50*3], str_2[50*3], str_3[50*3];
    for (int i = 0; i < 50*3; i++) {
        str_1[i] = str[i];
        str_2[i] = str[50*3 + i];
        str_3[i] = str[50*3*2 + i];
    }

    // Assign to str
    sprintf(str, "[%s, %s, %s]",
        str_1, str_2, str_3
    );
}
