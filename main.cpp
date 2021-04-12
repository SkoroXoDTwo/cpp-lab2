#include <cmath>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

#pragma pack(push, 1)
struct TGA {
    uint8_t id_len;
    uint8_t pal_type;
    uint8_t img_type;
    uint8_t pal_desc[5];
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t width;
    uint16_t height;
    uint8_t depth;
    uint8_t img_desc;
};
#pragma pack(pop)

constexpr uint16_t
IMG_WIDTH = 1920;
constexpr uint16_t
IMG_HEIGHT = 1080;
constexpr uint32_t
COL_BACKGROUND = 0xff000000;
constexpr uint32_t
COL_FOREGROUND = 0xff008000;

double sinc(double x) {
    if (x == 0) return 1.;
    return sin(x) / x;
}

double my_evil_function(double x, double y) {
    return sinc(hypot(x, y));
}

const double PI = acos(-1.0);
constexpr double rx = 40., ry = 40., rz = 500.;
constexpr double xMax = 40., xMin = -40.;
constexpr double yMax = 40., yMin = -40.;
constexpr double stepOne = 0.3, stepTwo = 0.01;

void floatingHorizon(std::vector <uint32_t> &pixels, int x, int y, uint32_t c, std::vector<int> &h) {
    if (x < 0 || x >= IMG_WIDTH) return;
    if (y > h[x]) return;

    h[x] = y;
    if (y < 0) return;

    if ((int) (y * IMG_WIDTH + x) <= (int) pixels.size()) {
        pixels[y * IMG_WIDTH + x] = c;
    }
}

int main() {
    std::vector <uint32_t> picture(IMG_WIDTH * IMG_HEIGHT);

    int sx = 0, sy = 0;
    double sz;

    std::vector<int> horizon;
    horizon.resize(IMG_WIDTH);

    for (auto &&a:horizon) {
        a = IMG_HEIGHT;
    }

    for (auto &&c:picture) {
        c = COL_BACKGROUND;
    }

    TGA hdr{};
    hdr.img_type = 2;
    hdr.width = IMG_WIDTH;
    hdr.height = IMG_HEIGHT;
    hdr.depth = 32;
    hdr.img_desc = 0x28;

    for (double i = xMax; i > xMin; i -= stepOne) {
        for (double j = yMax; j > yMin; j -= stepTwo) {
            sz = my_evil_function(i, j);
            sx = int(IMG_WIDTH / 2 - rx * i * cos(M_PI / 6) + ry * j * cos(M_PI / 6));
            sy = int(IMG_HEIGHT / 2 + rx * i * sin(M_PI / 6) + ry * j * sin(M_PI / 6) - rz * sz);
            floatingHorizon(picture, sx, sy, COL_FOREGROUND, horizon);
        }
    }

    for (auto &&a:horizon) {
        a = IMG_HEIGHT;
    }

    for (double i = xMax; i > xMin; i -= stepTwo) {
        for (double j = yMax; j > yMin; j -= stepOne) {
            sz = my_evil_function(i, j);
            sx = int(IMG_WIDTH / 2 - rx * i * cos(PI / 6) + ry * j * cos(PI / 6));
            sy = int(IMG_HEIGHT / 2 + rx * i * sin(PI / 6) + ry * j * sin(PI / 6) - rz * sz);
            floatingHorizon(picture, sx, sy, COL_FOREGROUND, horizon);
        }
    }

    std::ofstream tga_file{"output.tga", std::ios::binary};
    tga_file.write(reinterpret_cast<char *>(&hdr), sizeof(hdr));
    tga_file.write(reinterpret_cast<char *>(&picture[0]),
                   picture.size() * sizeof(uint32_t));
    tga_file.close();

    return 0;
}
