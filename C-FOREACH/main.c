#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int total = argc - 1;
    int l_total;
    char ***elm;
    int *l_count;

    //printf("total:%d\n", argc);
    //for (int i = 0; i < argc; i++) {
    //    printf("%d [%s]\n", i, argv[i]);
    //}

    elm = (char ***)calloc(total, sizeof(char **));
    l_count = (int *)calloc(total, sizeof(int));
    for (int l = 0; l < total; l++) {
        if (l == 0) {
            l_total = total;
            l_count[l] = l_total;
            elm[l] = (char **)calloc(l_total, sizeof(char *));
            for (int i = 0; i < l_total; i++) {
                elm[l][i] = argv[i + 1];

                printf("%s", elm[l][i]);
                if (i == l_total - 1) {
                    printf("\n");
                } else {
                    printf(" ");
                }
            }
        } else {
            l_total = (total - l + 1) * (total - l) / 2;
            l_count[l] = l_total;
            elm[l] = (char **)calloc(l_total, sizeof(char *));
            for (int i = 0, k = 0; i < l_count[l - 1] - 1; i++) {
                for (int j = i + l; j < total; j++) {
                    elm[l][k] = (char *)calloc(strlen(elm[l - 1][i]) + strlen(elm[0][j]) + 1, sizeof(char));
                    strcpy(elm[l][k], elm[l - 1][i]);
                    strcat(elm[l][k], elm[0][j]);
                    printf("%s", elm[l][k]);
                    if (k == l_total - 1) {
                        printf("\n");
                    } else {
                        printf(" ");
                    }
                    k++;
                }
            }
        }
    }
    return 0;
}