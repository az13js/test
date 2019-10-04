#include <stdio.h>
#include <stdlib.h>
#include "pearson.h"

double test(int len) {
    //int len = 10000; // 统计次数
    int seed = 1; // 随机数种子
    struct pear_data *pear_data;
    double seq;

    // 数组a和b用来保存每一次
    char *a, *b;

    a = (char*)calloc(len, sizeof (char));
    b = (char*)calloc(len, sizeof (char));

    srand(seed);
    for (int i = 0; i < len; i++) {
        a[i] = rand() & 0x01;
        b[i] = rand() & 0x01;
        if (len <= 10) {
            //printf("%d: %d,%d\n", i, a[i], b[i]);
        }
    }

    pear_data = pear_create_from_chars(len, a, b);
    seq = pear_get_chi_square(pear_data);
    //printf("%lf\n", seq);

    pear_free(pear_data);
    free(a);
    free(b);

    return seq;
}

int main() {
    double max = 0.0;
    int max_i = 0;
    double tmp;
    for (int i = 10; i < 60000; i++) {
        tmp = test(i);
        if (tmp > max) {
            printf("i: %d, s: %lf, believe: %lf\n", i, tmp, pear_believe(tmp));
            max = tmp;
            max_i = i;
        }
    }
    return 0;
}