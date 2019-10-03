/*
 *
 * 0x00 -> 0,0
 * 0x01 -> 0,1
 * 0x02 -> 1,0
 * 0x03 -> 1,1
 *
 */
#include <stdio.h>
#include <stdlib.h>

int main() {
    char x[4][4] = {"0,0", "0,1", "1,0", "1,1"};
    int total = 100000000;
    FILE *fp;
    srand(1);
    fp = fopen("RANDOM.csv", "w");
    fprintf(fp, "A,B\n");
    for (int i = 0; i < total; i++) {
        fprintf(fp, "%s\n", x[rand() % 4]);
    }
    fclose(fp);
    return 0;
}