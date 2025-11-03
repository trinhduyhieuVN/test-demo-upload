#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <iostream>
using namespace std;

// ------------------- THUẬT TOÁN BRESENHAM VẼ ĐƯỜNG THẲNG -------------------
void bresenhamLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int p = 2*dy - dx;
    int x, y;

    if (x1 > x2) {
        x = x2; y = y2; x2 = x1;
    } else {
        x = x1; y = y1;
    }

    putpixel(x, y, WHITE);

    while (x < x2) {
        x++;
        if (p < 0) p += 2*dy;
        else {
            y++;
            p += 2*(dy - dx);
        }
        putpixel(x, y, WHITE);
    }
}

// ------------------- THUẬT TOÁN MIDPOINT VẼ ĐƯỜNG TRÒN -------------------
void midpointCircle(int xc, int yc, int r) {
    int x = 0, y = r;
    int p = 1 - r;

    while (x <= y) {
        putpixel(xc + x, yc + y, WHITE);
        putpixel(xc - x, yc + y, WHITE);
        putpixel(xc + x, yc - y, WHITE);
        putpixel(xc - x, yc - y, WHITE);
        putpixel(xc + y, yc + x, WHITE);
        putpixel(xc - y, yc + x, WHITE);
        putpixel(xc + y, yc - x, WHITE);
        putpixel(xc - y, yc - x, WHITE);

        x++;
        if (p < 0) p += 2*x + 1;
        else {
            y--;
            p += 2*(x - y) + 1;
        }
    }
}

// ------------------- THUẬT TOÁN MIDPOINT VẼ ELLIPSE -------------------
void midpointEllipse(int xc, int yc, int rx, int ry) {
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Vùng 1
    d1 = (ry*ry) - (rx*rx*ry) + (0.25*rx*rx);
    dx = 2*ry*ry*x;
    dy = 2*rx*rx*y;

    while (dx < dy) {
        putpixel(xc + x, yc + y, WHITE);
        putpixel(xc - x, yc + y, WHITE);
        putpixel(xc + x, yc - y, WHITE);
        putpixel(xc - x, yc - y, WHITE);

        if (d1 < 0) {
            x++;
            dx += 2*ry*ry;
            d1 += dx + (ry*ry);
        } else {
            x++; y--;
            dx += 2*ry*ry;
            dy -= 2*rx*rx;
            d1 += dx - dy + (ry*ry);
        }
    }

    // Vùng 2
    d2 = ((ry*ry)*((x+0.5)*(x+0.5))) +
         ((rx*rx)*((y-1)*(y-1))) -
         (rx*rx*ry*ry);

    while (y >= 0) {
        putpixel(xc + x, yc + y, WHITE);
        putpixel(xc - x, yc + y, WHITE);
        putpixel(xc + x, yc - y, WHITE);
        putpixel(xc - x, yc - y, WHITE);

        if (d2 > 0) {
            y--;
            dy -= 2*rx*rx;
            d2 += (rx*rx) - dy;
        } else {
            y--; x++;
            dx += 2*ry*ry;
            dy -= 2*rx*rx;
            d2 += dx - dy + (rx*rx);
        }
    }
}

// ------------------- VẼ PARABOL, SIN, COS -------------------
void drawParabola(int xc, int yc, int a) {
    for (int x = -200; x <= 200; x++) {
        int y = a * x * x / 100;
        putpixel(xc + x, yc - y, WHITE);
    }
}

void drawSin(int xc, int yc) {
    for (int x = -300; x <= 300; x++) {
        int y = 50 * sin(x * 3.14 / 180);
        putpixel(xc + x, yc - y, WHITE);
    }
}

void drawCos(int xc, int yc) {
    for (int x = -300; x <= 300; x++) {
        int y = 50 * cos(x * 3.14 / 180);
        putpixel(xc + x, yc - y, WHITE);
    }
}

// ------------------- MAIN -------------------
int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    // Vẽ đường thẳng Bresenham
    bresenhamLine(100, 100, 300, 200);

    // Vẽ đường tròn Midpoint
    midpointCircle(200, 200, 80);

    // Vẽ ellipse Midpoint
    midpointEllipse(400, 300, 100, 60);

    // Vẽ Parabol
    drawParabola(300, 400, 1);

    // Vẽ sin(x), cos(x)
    drawSin(400, 200);
    drawCos(400, 300);

    getch();
    closegraph();
    return 0;
}
