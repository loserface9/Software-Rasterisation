#include <stdlib.h>
#include <string.h>

#include "make_frame.h"

#include "bitmap.h"


void make_bitmap(unsigned char *restrict bitmap, const unsigned char *restrict frame, const int WIN_WIDTH, const int WIN_HEIGHT) {
// *bitmap  - unsigned char [WIN_HEIGHT*WIN_WIDTH*4 + 54]
// *frame   - unsigned char [WIN_HEIGHT][WIN_WIDTH][3]

    // https://stackoverflow.com/a/47785639

    const unsigned long FILE_HEADER_SIZE = 14, INFO_HEADER_SIZE = 40;
    const unsigned long HEADER_SIZE = FILE_HEADER_SIZE + INFO_HEADER_SIZE;
    // const unsigned long FILE_SIZE = PIXEL_COUNT*4 + HEADER_SIZE;
    const unsigned long BYTES_PER_PIXEL = 4*sizeof(unsigned char);
    const unsigned long PIXEL_COUNT = WIN_HEIGHT*WIN_WIDTH;
    const unsigned long FILE_SIZE_BYTES = PIXEL_COUNT*BYTES_PER_PIXEL + (HEADER_SIZE)*sizeof(unsigned char);

    unsigned char *body = calloc(PIXEL_COUNT*4, sizeof(unsigned char));
    for (int yIdx = 0; yIdx < WIN_HEIGHT; yIdx++) {
        for (int xIdx = 0; xIdx < WIN_WIDTH; xIdx++) {
            // Calculate the index: row * num_cols + column
            copy_pixel_rgb(&body[yIdx*WIN_WIDTH*4 + xIdx*4], &frame[yIdx*WIN_WIDTH*3 + xIdx*3]);
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

    memcpy(bitmap,                    fileheader, FILE_HEADER_SIZE*sizeof(unsigned char));
    memcpy(&bitmap[FILE_HEADER_SIZE], infoheader, INFO_HEADER_SIZE*sizeof(unsigned char));
    memcpy(&bitmap[HEADER_SIZE],      body,       PIXEL_COUNT*BYTES_PER_PIXEL);
    free(body);
}
