#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct wd_world {
    int width;
    int height;
    int **map;
};

int** _wd_map_create(int width, int height) {
    int **map;
    map = (int**)calloc(width, sizeof (int*));
    for (int i = 0; i < width; i++) {
        map[i] = (int*)calloc(height, sizeof (int));
    }
    return map;
}

void _wd_map_show(int **map, int width, int height) {
    printf("map:\n");
    for (int h = height - 1; h > -1; h--) {
        for (int w = 0; w < width; w++) {
            printf("%d", map[w][h]);
            if (w > -1 && w < width - 1) {
                printf(",");
            }
        }
        printf("\n");
    }
}

int _wd_map_save(int **map, int width, int height, const char *file) {
    FILE *fp;
    int size = 0;
    fp = fopen(file, "w");
    size += fprintf(fp, "%d\n", width);
    size += fprintf(fp, "%d\n", height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size += fprintf(fp, "%d\n", map[x][y]);
        }
    }
    fclose(fp);
    return size;
}

void _wd_map_destory(int **map, int width) {
    for (int i = 0; i < width; i++) {
        free(map[i]);
    }
    free(map);
}

struct wd_world* wd_create(int width, int height) {
    struct wd_world* world;
    world = (struct wd_world*)malloc(sizeof (struct wd_world));
    (*world).width = width;
    (*world).height = height;
    (*world).map = _wd_map_create(width, height);
    return world;
}

void wd_destory(struct wd_world* world) {
    _wd_map_destory((*world).map, (*world).width);
    free(world);
}

void wd_show(struct wd_world* world, int w_width, int w_height, int x, int y) {
    for (int py = y + w_height - 1; py >= y; py--) {
        for (int px = x; px < x + w_width; px++) {
            if (0 == (*world).map[px][py]) {
                printf(" ");
            } else {
                printf("%d", (*world).map[px][py]);
            }
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    struct wd_world* world;
    int width = 1000, height = 1000, w_width = 40, w_height = 20;
    int now_x = 0, now_y = 0;
    char c;
    srand(2);
    world = wd_create(width, height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            (*world).map[x][y] = rand() % 2;
        }
    }
    while (1) {
        wd_show(world, w_width, w_height, now_x, now_y);
        printf("w,a,s,d:");
        c = getchar();
        getchar();
        if ('q' == c) {
            break;
        }
        if ('w' == c) {
            now_y++;
        }
        if ('a' == c) {
            now_x--;
        }
        if ('s' == c) {
            now_y--;
        }
        if ('d' == c) {
            now_x++;
        }
        if (now_x < 0) {
            now_x = 0;
        }
        if (now_y < 0) {
            now_y = 0;
        }
        if (now_x > width - w_width - 1) {
            now_x = width - w_width - 1;
        }
        if (now_y > height - w_height - 1) {
            now_y = height - w_height - 1;
        }
    }
    wd_destory(world);
    return 0;
}