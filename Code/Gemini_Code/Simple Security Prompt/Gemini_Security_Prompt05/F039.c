// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

int main() {
    Table *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = get_token(&p);
            char *r_s = get_token(&p);
            char *c_s = get_token(&p);
            if (name && r_s && c_s) {
                int rows = atoi(r_s);
                int cols = atoi(c_s);
                if (rows > 0 && cols > 0) {
                    char ***cells = malloc(rows * sizeof(char **));
                    for (int i = 0; i < rows; i++) {
                        cells[i] = malloc(cols * sizeof(char *));
                        for (int j = 0; j < cols; j++) {
                            cells[i][j] = strdup("EMPTY");
                        }
                    }
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Table *narr = realloc(arr, cap * sizeof(Table));
                        if (narr) arr = narr;
                    }
                    arr[count].name = strdup(name);
                    arr[count].rows = rows;
                    arr[count].cols = cols;
                    arr[count].cells = cells;
                    count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = get_token(&p);
            char *r_s = get_token(&p);
            char *c_s = get_token(&p);
            char *val = get_token(&p);
            if (name && r_s && c_s && val) {
                int r = atoi(r_s);
                int c = atoi(c_s);
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                            free(arr[i].cells[r][c]);
                            arr[i].cells[r][c] = strdup(val);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = get_token(&p);
            char *r_s = get_token(&p);
            char *c_s = get_token(&p);
            if (name && r_s && c_s) {
                int r = atoi(r_s);
                int c = atoi(c_s);
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                            printf("%s\n", arr[i].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        for (int r = 0; r < arr[i].rows; r++) {
                            for (int c = 0; c < arr[i].cols; c++) {
                                free(arr[i].cells[r][c]);
                            }
                            free(arr[i].cells[r]);
                        }
                        free(arr[i].cells);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int r = 0; r < arr[i].rows; r++) {
            for (int c = 0; c < arr[i].cols; c++) {
                free(arr[i].cells[r][c]);
            }
            free(arr[i].cells[r]);
        }
        free(arr[i].cells);
    }
    free(arr);
    return 0;
}
