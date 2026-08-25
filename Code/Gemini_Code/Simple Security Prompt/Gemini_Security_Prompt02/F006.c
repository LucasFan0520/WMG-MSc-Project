// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *item;
    int count;
} Inv;

int main() {
    Inv *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *item_start = line + 4;
            char *space = strchr(item_start, ' ');
            if (space) {
                *space = '\0';
                int cnt = atoi(space + 1);
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item_start) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    arr[found].count += cnt;
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(Inv));
                    }
                    arr[count].item = strdup(item_start);
                    arr[count].count = cnt;
                    count++;
                }
            }
        } else if (strncmp(line, "TAKE ", 5) == 0) {
            char *item_start = line + 5;
            char *space = strchr(item_start, ' ');
            if (space) {
                *space = '\0';
                int cnt = atoi(space + 1);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item_start) == 0) {
                        arr[i].count -= cnt;
                        if (arr[i].count < 0) arr[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *item_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item_start) == 0) {
                    free(arr[i].item);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *item_start = line + 6;
            int cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item_start) == 0) {
                    cnt = arr[i].count;
                    break;
                }
            }
            printf("%d\n", cnt);
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", arr[i].item, arr[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].item);
    }
    free(arr);
    return 0;
}
