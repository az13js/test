/*
 * 利用C语言的速度进行相关性的测试
 *
 * 由于C语言编写的程序效率很高，所以对大量事件进行分析的速度比其它语言快多了
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * 具有多行多列的二维表
 *
 * 属性
 * data 数据，格式 data[column][row]
 * rows 行数
 * columns 列数
 * head 表头
 */
struct pal_table {
    char **data;
    int rows;
    int columns;
    char **head;
};

/*
 * 读取文件中的表格
 *
 * 参数列表
 * file_name 文件名
 *
 * 返回值
 * pal_table 指针
 */
struct pal_table* pal_read(const char *file_name) {
    struct pal_table *tb = (struct pal_table*)malloc(sizeof(struct pal_table));
    FILE *fp = fopen(file_name, "r");
    int columns, rows, tmp;

    tmp = fscanf(fp, "%d,%d", &((*tb).columns), &((*tb).rows));
    columns = (*tb).columns;
    rows = (*tb).rows;

    (*tb).head = (char**)calloc(columns, sizeof(char*));
    for (int i = 0; i < columns; i++) {
        (*tb).head[i] = (char*)calloc(255, sizeof(char));
        tmp = fscanf(fp, "%s", (*tb).head[i]);
    }

    (*tb).data = (char**)calloc(columns, sizeof(char*));
    for (int column = 0; column < columns; column++) {
        (*tb).data[column] = (char*)calloc(rows, sizeof(char));
    }

    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            tmp = fscanf(fp, "%d", (int*)&((*tb).data[column][row]));
        }
    }

    fclose(fp);
    return tb;
}

/*
 * 在控制台打印表
 *
 * 参数列表
 * table 一个 pal_table 结构体指针
 *
 * 返回值
 * void
 */
void pal_print(struct pal_table *table) {
    char **head = (*(table)).head;
    int columns = (*(table)).columns;
    int rows = (*(table)).rows;
    char **data = (*(table)).data;
    printf("%d,%d\n", columns, rows);
    for (int column = 0; column < columns; column++) {
        printf("%s", head[column]);
        if (column < columns - 1) {
            printf(" ");
        }
    }
    printf("\n");
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            printf("%d", data[column][row]);
            if (column < columns - 1) {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void pal_free(struct pal_table *table) {
    for (int i = 0; i < (*table).columns; i++) {
        free((*table).head[i]);
    }
    free((*table).head);
    for (int i = 0; i < (*table).columns; i++) {
        free((*table).data[i]);
    }
    free((*table).data);
    free(table);
}

int pal_save(struct pal_table *table, const char *file_name) {
    FILE *fp = fopen(file_name, "w");
    if (NULL == fp) {
        return -1;
    }
    fprintf(fp, "%d,%d\n", (*table).columns, (*table).rows);
    for (int i = 0; i < (*table).columns; i++) {
        fprintf(fp, "%s", (*table).head[i]);
        if (i < (*table).columns - 1) {
            fprintf(fp, " ");
        } else {
            fprintf(fp, "\n");
        }
    }
    for (int row = 0; row < (*table).rows; row++) {
        for (int column = 0; column < (*table).columns; column++) {
            fprintf(fp, "%d", (*table).data[column][row]);
            if (column < (*table).columns - 1) {
                fprintf(fp, " ");
            } else {
                fprintf(fp, "\n");
            }
        }
    }
    fclose(fp);
    return 0;
}

int main() {
    int d;
    struct pal_table *table;
    table = pal_read("data.csv");
    pal_save(table, "data_s.csv");
    pal_print(table);
    pal_free(table);
    return 0;
}