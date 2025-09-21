#include <SDL3/SDL_stdinc.h>
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080
#define x_range (3)
#define y_range (x_range / ((double) WIN_WIDTH / WIN_HEIGHT))


struct Triangle {
    double A[3], B[3], C[3];
    int color;
};


void array3_to_str(char str[], double P[3]);
void Triangle_to_str(char str[], struct Triangle *triangle);
void print_array2d(double *A, int rows, int cols);
void arraysub(double *w, double *u, double *v, int length);
void matinv2(double A[2][2]);
int matmul(double *C, double *A, double*B, int A_rows, int A_cols, int B_rows, int B_cols);
int create_rotation_matrix(double rot_mat[3][3], double theta_x, double theta_y, double theta_z);
void transform_triangle(struct Triangle *restrict triangle, double trans_mat[3][3]);
void rasterise (
    int *restrict buffer, double *restrict z_buffer,
    struct Triangle *restrict triangle, double x_positions[WIN_WIDTH], double y_positions[WIN_HEIGHT]
);
void clear_buffers (int *restrict buffer, double *restrict z_buffer);
void print_buffer(int *restrict buffer);
int make_frame(unsigned char *restrict frame, const int *restrict buffer);
void make_bitmap(unsigned char *restrict bitmap, const unsigned char *restrict frame);


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

    int *buffer = malloc(WIN_HEIGHT*WIN_WIDTH*sizeof(int));
    double *z_buffer = malloc(WIN_HEIGHT*WIN_WIDTH*sizeof(double));
    clear_buffers(buffer, z_buffer);

    double rot_mat[3][3];
    // create_rotation_matrix(rot_mat, -M_PI/(12*16), M_PI/(12*8), 0);
    create_rotation_matrix(rot_mat, -M_PI/(6), M_PI/(3), 0);

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
    //     print_buffer(buffer);
    // }

    unsigned char *frame = calloc(WIN_HEIGHT*WIN_WIDTH*3, sizeof(unsigned char));
    make_frame(frame, buffer);
    
    const unsigned long FILE_SIZE = WIN_HEIGHT*WIN_WIDTH*4 + 14 + 40;
    const unsigned long FILE_SIZE_BYTES = FILE_SIZE*sizeof(unsigned char);
    unsigned char *bitmap = calloc(FILE_SIZE, sizeof(unsigned char));
    make_bitmap(bitmap, frame);

    char filename[] = "bitmap.bmp";
    FILE *fp = fopen(filename, "w");
    fwrite(bitmap, FILE_SIZE_BYTES, 1, fp);
    fclose(fp);

    free(frame);
    free(bitmap);
    
    return 0;
}


void make_bitmap(unsigned char *restrict bitmap, const unsigned char *restrict frame) {
// *bitmap  - unsigned char [WIN_HEIGHT*WIN_WIDTH*4 + 54]
// *frame   - unsigned char [WIN_HEIGHT][WIN_WIDTH][3]

    // https://stackoverflow.com/a/47785639

    const unsigned long FILE_HEADER_SIZE = 14, INFO_HEADER_SIZE = 40;
    const unsigned long HEADER_SIZE = FILE_HEADER_SIZE + INFO_HEADER_SIZE;
    const unsigned long PIXEL_COUNT = WIN_HEIGHT*WIN_WIDTH;
    // const unsigned long FILE_SIZE = PIXEL_COUNT*4 + HEADER_SIZE;
    const unsigned long BYTES_PER_PIXEL = 4*sizeof(unsigned char);
    const unsigned long FILE_SIZE_BYTES = PIXEL_COUNT*BYTES_PER_PIXEL + (HEADER_SIZE)*sizeof(unsigned char);

    unsigned char *body = calloc(PIXEL_COUNT*4, sizeof(unsigned char));
    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            // Calculate the index: row * num_cols + column
            body[yIdx*WIN_WIDTH*4 + xIdx*4 + 0] = frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0];
            body[yIdx*WIN_WIDTH*4 + xIdx*4 + 1] = frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1];
            body[yIdx*WIN_WIDTH*4 + xIdx*4 + 2] = frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2];
            body[yIdx*WIN_WIDTH*4 + xIdx*4 + 3] = 0;
        }
    }

    unsigned char fileheader[14] = {
        'B', 'M',
        0, 0, 0, 0, // File size (bytes)
        0, 0, 0, 0, // Reserved
        0, 0, 0, 0, // Start of pixel array
    };

    fileheader[ 2] = (unsigned char)(FILE_SIZE_BYTES      );
    fileheader[ 3] = (unsigned char)(FILE_SIZE_BYTES >>  8);
    fileheader[ 4] = (unsigned char)(FILE_SIZE_BYTES >> 16);
    fileheader[ 5] = (unsigned char)(FILE_SIZE_BYTES >> 24);
    fileheader[10] = (unsigned char)(HEADER_SIZE);

    unsigned char infoheader[40] = {
        0,0,0,0, // info header size
        0,0,0,0, // image width
        0,0,0,0, // image height
        0,0,     // number of color planes
        0,0,     // bits per pixel
        0,0,0,0, // compression
        0,0,0,0, // image size
        0,0,0,0, // horizontal resolution
        0,0,0,0, // vertical resolution
        0,0,0,0, // colors in color table
        0,0,0,0, // important color count
    };

    infoheader[ 0] = (unsigned char)(INFO_HEADER_SIZE);
    infoheader[ 4] = (unsigned char)(WIN_WIDTH      );
    infoheader[ 5] = (unsigned char)(WIN_WIDTH >>  8);
    infoheader[ 6] = (unsigned char)(WIN_WIDTH >> 16);
    infoheader[ 7] = (unsigned char)(WIN_WIDTH >> 24);
    infoheader[ 8] = (unsigned char)(WIN_HEIGHT      );
    infoheader[ 9] = (unsigned char)(WIN_HEIGHT >>  8);
    infoheader[10] = (unsigned char)(WIN_HEIGHT >> 16);
    infoheader[11] = (unsigned char)(WIN_HEIGHT >> 24);
    infoheader[12] = (unsigned char)(1);
    infoheader[14] = (unsigned char)(BYTES_PER_PIXEL*8);

    memcpy(bitmap, fileheader, FILE_HEADER_SIZE*sizeof(unsigned char));
    memcpy(&bitmap[FILE_HEADER_SIZE], infoheader, INFO_HEADER_SIZE*sizeof(unsigned char));
    memcpy(&bitmap[HEADER_SIZE], (char *)body, PIXEL_COUNT*BYTES_PER_PIXEL);
    free(body);
}


int make_frame(unsigned char *restrict frame, const int *restrict buffer) {
// *buffer  - int [WIN_HEIGHT][WIN_WIDTH]
// *frame   - unsigned char [WIN_HEIGHT][WIN_WIDTH][3]

    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            if (buffer[yIdx*WIN_WIDTH + xIdx] > 0) {
                switch (buffer[yIdx*WIN_WIDTH + xIdx]) {
                    case 2: // R
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 0;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 0;
                        break;
                    case 3: // G
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 0;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 0;
                        break;
                    case 4: // Y
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 0;
                        break;
                    case 5: // B
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 0;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 0;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 255;
                        break;
                    case 6: // M
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 0;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 255;
                        break;
                    case 7: // C
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 0;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 255;
                        break;
                    case 1: case 8: // W
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 255;
                        frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 255;
                        break;
                }
            } else {
                frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 0] = 0;
                frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 1] = 0;
                frame[yIdx*WIN_WIDTH*3 + xIdx*3 + 2] = 0;
            }
        }
    }
    return 0;
}


void print_buffer(int *restrict buffer) {
// *buffer  - [WIN_HEIGHT][WIN_WIDTH]

    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            if (buffer[yIdx*WIN_WIDTH + xIdx] > 0) {
                switch (buffer[yIdx*WIN_WIDTH + xIdx]) {
                    case 2: printf("\033[0;31m"); break;
                    case 3: printf("\033[0;32m"); break;
                    case 4: printf("\033[0;33m"); break;
                    case 5: printf("\033[0;34m"); break;
                    case 6: printf("\033[0;35m"); break;
                    case 7: printf("\033[0;36m"); break;
                    case 8: printf("\033[0m"); break;
                }
                printf("█");
            } else {
                printf("\033[0m");
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\033[0m");
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
    struct Triangle *restrict triangle, double x_positions[WIN_WIDTH], double y_positions[WIN_HEIGHT]
) {
// *buffer    - int [WIN_HEIGHT][WIN_WIDTH]
// *z_buffer  - double [WIN_HEIGHT][WIN_WIDTH]

    double A[2] = {triangle->A[0], triangle->A[1]};
    double B[2] = {triangle->B[0], triangle->B[1]};
    double C[2] = {triangle->C[0], triangle->C[1]};
    double AB[2], AC[2], AB_3d[3], AC_3d[3];
    arraysub(AB, B, A, 2);
    arraysub(AC, C, A, 2);
    arraysub(AB_3d, triangle->B, triangle->A, 3);
    arraysub(AC_3d, triangle->C, triangle->A, 3);
    double n[3] = {
        AB_3d[1]*AC_3d[2] - AB_3d[2]*AC_3d[1],
        AB_3d[2]*AC_3d[0] - AB_3d[0]*AC_3d[2],
        AB_3d[0]*AC_3d[1] - AB_3d[1]*AC_3d[0]
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
            double AP[2];
            arraysub(AP, P, A, 2);

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


int create_rotation_matrix(double rot_mat[3][3], double theta_x, double theta_y, double theta_z) {
    double rot_mat_x[3][3] = {
        {1, 0, 0},
        {0, cos(theta_x), -sin(theta_x)},
        {0, sin(theta_x), cos(theta_x)}
    };
    double rot_mat_y[3][3] = {
        {cos(theta_y), 0, sin(theta_y)},
        {0, 1, 0},
        {-sin(theta_y), 0, cos(theta_y)}
    };
    double rot_mat_z[3][3] = {
        {cos(theta_z), -sin(theta_z), 0},
        {sin(theta_z), cos(theta_z), 0},
        {0, 0, 1}
    };

    double rot_mat_zy[3][3];
    matmul((double *)rot_mat_zy, (double *)rot_mat_z, (double *)rot_mat_y, 3, 3, 3, 3);
    matmul((double *)rot_mat, (double *)rot_mat_zy, (double *)rot_mat_x, 3, 3, 3, 3);

    return 0;
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


void arraysub (double *w, double *u, double *v, int length) {
    // Assigns w = u - v
    for (int i = 0; i < length; i++) {
        w[i] = u[i] - v[i];
    }
}


void matinv2 (double A[2][2]) {
    // Invert a matrix
    const double denom = (A[0][0] * A[1][1]) - (A[0][1] * A[1][0]);
    const double temp[2][2] = {{A[0][0], A[0][1]}, {A[1][0], A[1][1]}};
    A[0][0] = temp[1][1] / denom;
    A[0][1] = - temp[0][1] / denom;
    A[1][0] = - temp[1][0] / denom;
    A[1][1] = temp[0][0] / denom;
}


int matmul(double *C, double *A, double *B, int A_rows, int A_cols, int B_rows, int B_cols) {
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


void array3_to_str (char str[], double P[3]) {
    // str should be preallocated length 50*3
    sprintf(str, "[%lf, %lf, %lf]",
        P[0], P[1], P[2]
    );
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
