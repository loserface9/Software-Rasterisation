#ifndef MAKE_FRAME_H
#define MAKE_FRAME_H

int make_frame(unsigned char *frame, const int *buffer, const int WIN_WIDTH, const int WIN_HEIGHT);
void set_pixel_rgb (unsigned char *pixel, unsigned char r, unsigned char g, unsigned char b);
void copy_pixel_rgb (unsigned char *destination, const unsigned char *origin);

#endif
