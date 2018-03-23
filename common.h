#include <math.h>
#include <stdio.h>

/* write_bmp_header():
 *
 *   In:
 *      f: A file open for writing ('w')
 *      (width, height): image dimensions
 *
 *   Return:
 *      0 on success, -1 on failure
 *
 */

int write_bmp_header(FILE *f, int width, int height);


/*
 * compute_julia_pixel(): compute RBG values of a pixel in a
 *                        particular Julia set image.
 *
 *  In:
 *      (x,y):            pixel coordinates
 *      (width, height):  image dimensions
 *      tint_bias:        a float to "tweak" the tint (1.0 is "no tint")
 *  Out:
 *      rgb: an already-allocated 3-byte array into which R, G, and B
 *           values are written.
 *
 *  Return:
 *      0 in success, -1 on failure
 *
 */

int compute_julia_pixel(int x, int y, int width, int height, float tint_bias, unsigned char *rgb);


/* save_julia_set(): Save data as a bmp file
 *
 *   In:
 *      name: name of file to save
 *      (width, height): image dimensions
 *      data: array with rgb data to save
 *
 *   Return:
 *      0 on success, -1 on failure
 *
 */
 
int save_julia_set(char* name, int width, int height, unsigned char* data);
