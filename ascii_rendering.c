#include "ascii_rendering.h"
#include "constants.h"

#include <stdio.h>


void output_buffer_as_ascii(int *restrict buffer) {
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
