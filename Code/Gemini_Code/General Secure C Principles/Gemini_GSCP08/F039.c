// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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

typedef struct {
    char *name;
    long rows;
    long cols;
    char **cells;
} CountedTable;

int main(void) {
    CountedTable *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "TABLES") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *r_s = p2 + 1;
                char *p3 = strchr(r_s, ' ');
                if (p3) {
                    *p3 = '\0';
                    long rows = strtol(r_s, NULL, 10);
                    long cols = strtol(p3 + 1, NULL, 10);
                    if (rows > 0 && cols > 0) {
                        char **cells = calloc(rows * cols, sizeof(char *));
                        if (cells) {
                            if (count >= cap) {
                                size_t ncap = cap == 0 ? 4 : cap * 2;
                                CountedTable *narr = realloc(arr, ncap * sizeof(CountedTable));
                                if (narr) { arr = narr; cap = ncap; }
                            }
                            if (count < cap) {
                                arr[count].name = strdup(name);
                                arr[count].rows = rows;
                                arr[count].cols = cols;
                                arr[count].cells = cells;
                                count++;
                            } else {
                                free(cells);
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *r_s = p2 + 1;
                char *p3 = strchr(r_s, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *c_s = p3 + 1;
                    char *p4 = strchr(c_s, ' ');
                    if (p4) {
                        *p4 = '\0';
                        char *val = p4 + 1;
                        long r = strtol(r_s, NULL, 10);
                        long c = strtol(c_s, NULL, 10);
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(arr[i].name, name) == 0) {
                                if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                                    size_t idx = (size_t)(r * arr[i].cols + c);
                                    free(arr[i].cells[idx]);
                                    arr[i].cells[idx] = strdup(val);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *r_s = p2 + 1;
                char *c_s = strchr(r_s, ' ');
                if (c_s) {
                    *c_s = '\0';
                    long r = strtol(r_s, NULL, 10);
                    long c = strtol(c_s + 1, NULL, 10);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                                size_t idx = (size_t)(r * arr[i].cols + c);
                                if (arr[i].cells[idx]) {
                                    printf("%s\n", arr[i].cells[idx]);
                                } else {
                                    printf("EMPTY\n");
                                }
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (long j = 0; j < arr[i].rows * arr[i].cols; j++) {
                        free(arr[i].cells[j]);
                    }
                    free(arr[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (long j = 0; j < arr[i].rows * arr[i].cols; j++) {
            free(arr[i].cells[j]);
        }
        free(arr[i].cells);
    }
    free(arr);
    return 0;
}
