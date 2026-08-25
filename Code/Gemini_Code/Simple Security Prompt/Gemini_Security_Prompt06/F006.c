// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *item;
    int count;
} InventoryItem;

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    InventoryItem *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", list[i].item, list[i].count);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *item = rest;
                int val = atoi(p2 + 1);
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].item, item) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "ADD") == 0) {
                    if (found != -1) {
                        list[found].count += val;
                    } else {
                        list = realloc(list, (count + 1) * sizeof(InventoryItem));
                        list[count].item = strdup(item);
                        list[count].count = val;
                        count++;
                    }
                } else {
                    if (found != -1) {
                        list[found].count -= val;
                        if (list[found].count < 0) {
                            list[found].count = 0;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].item);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(InventoryItem));
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].item, item) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%d\n", list[found].count);
            } else {
                printf("0\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].item);
    }
    free(list);
    return 0;
}