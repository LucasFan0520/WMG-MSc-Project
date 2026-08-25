// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

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
    CountedTable *list = NULL;
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
            if (strcmp(line, "TABLES") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", list[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "TABLE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    int r = atoi(rest2);
                    int c = atoi(p3 + 1);
                    if (r > 0 && c > 0) {
                        list = realloc(list, (count + 1) * sizeof(CountedTable));
                        list[count].name = strdup(name);
                        list[count].rows = r;
                        list[count].cols = c;
                        list[count].cells = malloc(r * sizeof(char **));
                        for (int i = 0; i < r; i++) {
                            list[count].cells[i] = malloc(c * sizeof(char *));
                            for (int j = 0; j < c; j++) {
                                list[count].cells[i][j] = strdup("EMPTY");
                            }
                        }
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    int r = atoi(rest2);
                    char *rest3 = p3 + 1;
                    char *p4 = strchr(rest3, ' ');
                    if (p4) {
                        *p4 = '\0';
                        int c = atoi(rest3);
                        char *val = p4 + 1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(list[i].name, name) == 0) {
                                if (r >= 0 && r < list[i].rows && c >= 0 && c < list[i].cols) {
                                    free(list[i].cells[r][c]);
                                    list[i].cells[r][c] = strdup(val);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    int r = atoi(rest2);
                    int c = atoi(p3 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].name, name) == 0) {
                            if (r >= 0 && r < list[i].rows && c >= 0 && c < list[i].cols) {
                                printf("%s\n", list[i].cells[r][c]);
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                for (int i = 0; i < list[found].rows; i++) {
                    for (int j = 0; j < list[found].cols; j++) {
                        free(list[found].cells[i][j]);
                    }
                    free(list[found].cells[i]);
                }
                free(list[found].cells);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(CountedTable));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        for (int r = 0; r < list[i].rows; r++) {
            for (int c = 0; c < list[i].cols; c++) {
                free(list[i].cells[r][c]);
            }
            free(list[i].cells[r]);
        }
        free(list[i].cells);
    }
    free(list);
    return 0;
}