#include "constants.h"

#include "make_frame.h"

int make_frame(unsigned char *restrict frame, const int *restrict buffer) {
// *buffer  - int [WIN_HEIGHT][WIN_WIDTH]
// *frame   - unsigned char [WIN_HEIGHT][WIN_WIDTH][3]

    int row_start_idx;
    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        row_start_idx = yIdx*WIN_WIDTH*3;
        for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            if (buffer[yIdx*WIN_WIDTH + xIdx] > 0) {
                switch (buffer[yIdx*WIN_WIDTH + xIdx]) {
                    case 2: // R
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 255, 0, 0);
                        break;
                    case 3: // G
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 0, 255, 0);
                        break;
                    case 4: // Y
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 255, 255, 0);
                        break;
                    case 5: // B
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 0, 0, 255);
                        break;
                    case 6: // M
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 255, 0, 255);
                        break;
                    case 7: // C
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 0, 255, 255);
                        break;
                    case 1: case 8: // W
                        set_pixel_rgb(&frame[row_start_idx + xIdx*3], 255, 255, 255);
                        break;
                }
            } else {
                set_pixel_rgb(&frame[row_start_idx + xIdx*3], 0, 0, 0);
            }
        }
    }
    return 0;
}


inline void set_pixel_rgb (unsigned char *restrict pixel, unsigned char r, unsigned char g, unsigned char b) {
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
}


inline void copy_pixel_rgb (unsigned char *restrict destination, const unsigned char *restrict origin) {
// Do not let destination and origin overlap

    destination[0] = origin[0];
    destination[1] = origin[1];
    destination[2] = origin[2];
}
