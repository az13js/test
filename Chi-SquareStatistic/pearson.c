#include "pearson.h"

struct pear_data* pear_create_from_chars(int len, const char *a, const char *b) {
    struct pear_data *pear_data = (struct pear_data *)malloc(sizeof (struct pear_data));
    (*pear_data).len = len;
    (*pear_data).a = (char *)calloc(len, sizeof(char));
    (*pear_data).b = (char *)calloc(len, sizeof(char));
    memcpy((*pear_data).a, a, len * sizeof(char));
    memcpy((*pear_data).b, b, len * sizeof(char));
    (*pear_data).type = 0;
    return pear_data;
}

double pear_get_chi_square(struct pear_data *pear_data) {
    /*
     * A B C
     * D E F
     * G H I
     */
    int A = 0, B = 0, C = 0, D = 0, E = 0, F = 0, G = 0, H = 0, I = 0;
    char *a = (*pear_data).a;
    char *b = (*pear_data).b;
    I = (*pear_data).len;
    for (int i = 0; i < I; i++) {
        if (a[i] && b[i]) {
            A++;
        }
        if (!a[i] && b[i]) {
            B++;
        }
        if (a[i] && !b[i]) {
            D++;
        }
        if (!a[i] && !b[i]) {
            E++;
        }
    }
    C = A + B;
    F = D + E;
    G = A + D;
    H = B + E;
    double squre = (double)(A * E - B * D);
    return squre / C / F * squre / G / H * I;
}

void pear_free(struct pear_data *pear_data) {
    if (0 == (*pear_data).type) {
        free((*pear_data).a);
        free((*pear_data).b);
    }
    free(pear_data);
}

double pear_believe(double chi_square) {
    double p[] = {0.5, 0.4, 0.25, 0.15, 0.1, 0.05, 0.025, 0.01, 0.005, 0.001};
    double x[] = {0.455, 0.708, 1.323, 2.072, 2.706, 3.841, 5.024, 6.635, 7.879, 10.828};
    if (chi_square < x[0]) {
        return 0;
    }
    for (int i = 0; i < 9; i++) {
        if (chi_square >= x[i] && chi_square < x[i + 1]) {
            return 1 - p[i];
        }
    }
    return 0.999;
}