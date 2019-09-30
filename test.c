/*
 * 单比特频数检测
 *
 * Author: az13js <1654602334@qq.com>
 *
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main() {
    int max = 100;
    char *charArray = (char*)calloc(max, sizeof(char));
    srand(1);

    for (int i = 0; i < max; i++) {
        charArray[i] = rand() & 0x01;
    }

    /* 第一步，转化0为-1 */
    for (int i = 0; i < max; i++) {
        charArray[i] = charArray[i] == 0 ? -1 : 1;
    }

    /* 第二步，对序列进行累加 */
    int sum = 0;
    for (int i = 0; i < max; i++) {
        sum += charArray[i];
    }
    printf("sum:%d\n", sum);

    /* 第三步，计算统计值 */
    double v = (double)abs(sum) / sqrt((double)max);
    printf("v:%lf\n", v);

    /* 最后一步，计算P值 */
    double P_value = erfc(v / sqrt(2));
    printf("P-value:%lf\n", P_value);

    double a = 0.01;
    if (P_value >= a) {
        printf("Random.\n");
    } else {
        printf("Not random.\n");
    }

    free(charArray);
    return 0;
}