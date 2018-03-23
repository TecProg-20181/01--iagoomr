#include <stdio.h>

typedef struct _pixel {
    unsigned short int r;
    unsigned short int g;
    unsigned short int b;
} Pixel;

typedef struct _image {
    // [width][height][rgb]
    // 0 -> r
    // 1 -> g
    // 2 -> b
    unsigned short int pixel[512][512][3];
    unsigned int w;
    unsigned int h;
} Image;


int max(int a, int b) {
    if (a > b)
        return a;
    return b;
}

int equal_pixel(Pixel p1, Pixel p2) {
    if (p1.r == p2.r &&
        p1.g == p2.g &&
        p1.b == p2.b)
        return 1;
    return 0;
}


Image grey_scale(Image img) {
    /*for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            print("%u", img.pixel[i][j][0] + img.pixel[i][j][1] + img.pixel[i][j][2]);
        }
    }*/

    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            int average = img.pixel[i][j][0] +
                        img.pixel[i][j][1] +
                        img.pixel[i][j][2];
            average /= 3;
            img.pixel[i][j][0] = average;
            img.pixel[i][j][1] = average;
            img.pixel[i][j][2] = average;
        }
    }

    return img;
}

void blur(unsigned int h, unsigned short int pixel[512][512][3], int T, unsigned int w) {
    for (unsigned int i = 0; i < h; ++i) {
        for (unsigned int j = 0; j < w; ++j) {
            Pixel average = {0, 0, 0};

            int minor_h = (h - 1 > i + T/2) ? i + T/2 : h - 1;
            int min_w = (w - 1 > j + T/2) ? j + T/2 : w - 1;
            for(int x = (0 > i - T/2 ? 0 : i - T/2); x <= minor_h; ++x) {
                for(int y = (0 > j - T/2 ? 0 : j - T/2); y <= min_w; ++y) {
                    average.r += pixel[x][y][0];
                    average.g += pixel[x][y][1];
                    average.b += pixel[x][y][2];
                }
            }

            // printf("%u", average.r)
            average.r /= T * T;
            average.g /= T * T;
            average.b /= T * T;

            pixel[i][j][0] = average.r;
            pixel[i][j][1] = average.g;
            pixel[i][j][2] = average.b;
        }
    }
}

Image rotate90right(Image img) {
    Image rotate;

    rotate.w = img.h;
    rotate.h = img.w;

    for (unsigned int i = 0, y = 0; i < rotate.h; ++i, ++y) {
        for (int j = rotate.w - 1, x = 0; j >= 0; --j, ++x) {
            rotate.pixel[i][j][0] = img.pixel[x][y][0];
            rotate.pixel[i][j][1] = img.pixel[x][y][1];
            rotate.pixel[i][j][2] = img.pixel[x][y][2];
        }
    }

    return rotate;
}

void color_inversion(unsigned short int pixel[512][512][3],
                    unsigned int w, unsigned int h) {
    for (unsigned int i = 0; i < h; ++i) {
        for (unsigned int j = 0; j < w; ++j) {
            pixel[i][j][0] = 255 - pixel[i][j][0];
            pixel[i][j][1] = 255 - pixel[i][j][1];
            pixel[i][j][2] = 255 - pixel[i][j][2];
        }
    }
}

Image cut_image(Image img, int x, int y, int w, int h) {
    Image cut;

    cut.w = w;
    cut.h = h;

    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            cut.pixel[i][j][0] = img.pixel[i + y][j + x][0];
            cut.pixel[i][j][1] = img.pixel[i + y][j + x][1];
            cut.pixel[i][j][2] = img.pixel[i + y][j + x][2];
        }
    }

    return cut;
}


int main() {
    Image img;

    // read type of image
    char p3[4];
    scanf("%s", p3);

    // read width height and color of image
    int max_color;
    scanf("%u %u %d", &img.w, &img.h, &max_color);

    // read all pixels of image
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            scanf("%hu %hu %hu", &img.pixel[i][j][0],
                                 &img.pixel[i][j][1],
                                 &img.pixel[i][j][2]);

        }
    }

    int n_choice;
    scanf("%d", &n_choice);

    for(int i = 0; i < n_choice; ++i) {
        int choice;
        scanf("%d", &choice);

        switch(choice) {
            case 1: { // grey scale
                img = grey_scale(img);
                break;
            }
            case 2: { // sepia filter
                for (unsigned int x = 0; x < img.h; ++x) {
                    for (unsigned int j = 0; j < img.w; ++j) {
                        unsigned short int pixel[3];
                        pixel[0] = img.pixel[x][j][0];
                        pixel[1] = img.pixel[x][j][1];
                        pixel[2] = img.pixel[x][j][2];

                        int p =  pixel[0] * .393 + pixel[1] * .769 + pixel[2] * .189;
                        int minor_r = (255 >  p) ? p : 255;
                        img.pixel[x][j][0] = minor_r;

                        p =  pixel[0] * .349 + pixel[1] * .686 + pixel[2] * .168;
                        minor_r = (255 >  p) ? p : 255;
                        img.pixel[x][j][1] = minor_r;

                        p =  pixel[0] * .272 + pixel[1] * .534 + pixel[2] * .131;
                        minor_r = (255 >  p) ? p : 255;
                        img.pixel[x][j][2] = minor_r;
                    }
                }

                break;
            }
            case 3: { // Blur
                int size = 0;
                scanf("%d", &size);
                blur(img.h, img.pixel, size, img.w);
                break;
            }
            case 4: { // rotate
                int how_many_times = 0;
                scanf("%d", &how_many_times);
                how_many_times %= 4;
                for (int j = 0; j < how_many_times; ++j) {
                    img = rotate90right(img);
                }
                break;
            }
            case 5: { // mirroring
                int horizontal = 0;
                scanf("%d", &horizontal);

                int w = img.w, h = img.h;

                if (horizontal == 1) w /= 2;
                else h /= 2;

                for (int i2 = 0; i2 < h; ++i2) {
                    for (int j = 0; j < w; ++j) {
                        int x = i2, y = j;

                        if (horizontal == 1) y = img.w - 1 - j;
                        else x = img.h - 1 - i2;

                        Pixel aux1;
                        aux1.r = img.pixel[i2][j][0];
                        aux1.g = img.pixel[i2][j][1];
                        aux1.b = img.pixel[i2][j][2];

                        img.pixel[i2][j][0] = img.pixel[x][y][0];
                        img.pixel[i2][j][1] = img.pixel[x][y][1];
                        img.pixel[i2][j][2] = img.pixel[x][y][2];

                        img.pixel[x][y][0] = aux1.r;
                        img.pixel[x][y][1] = aux1.g;
                        img.pixel[x][y][2] = aux1.b;
                    }
                }
                break;
            }
            case 6: { // color inversion
                color_inversion(img.pixel, img.w, img.h);
                break;
            }
            case 7: { // cut Image
                int x, y;
                scanf("%d %d", &x, &y);
                int w, h;
                scanf("%d %d", &w, &h);

                img = cut_image(img, x, y, w, h);
                break;
            }
        }

    }

    // print type of image
    printf("P3\n");
    // print width height and color of image
    printf("%u %u\n255\n", img.w, img.h);

    // print pixels of image
    for (unsigned int i = 0; i < img.h; ++i) {
        for (unsigned int j = 0; j < img.w; ++j) {
            printf("%hu %hu %hu ", img.pixel[i][j][0],
                                   img.pixel[i][j][1],
                                   img.pixel[i][j][2]);

        }
        printf("\n");
    }
    return 0;
}
