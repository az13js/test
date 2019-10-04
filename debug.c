#include <stdio.h>
#include <math.h>

int main() {
    double r, circum, area;
    printf("Input r:\n");
    scanf("%lf", &r);
    circum = 2 * M_PI * r;
    area = M_PI * r * r;
    printf("%f,%f\n", circum, area);
    printf("%7.2f,%7.2f\n", circum, area);
    return 0;
}