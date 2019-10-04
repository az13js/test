#ifndef PEARSON_C_LIB
#define PEARSON_C_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pear_data {
    char *a;
    char *b;
    int len;
    int type;
};

struct pear_data* pear_create_from_chars(int len, const char *a, const char *b);
double pear_get_chi_square(struct pear_data *pear_data);
void pear_free(struct pear_data *pear_data);
double pear_believe(double chi_square);

#endif