// F039.c
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
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table;

int main() {
    Table *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "TABLE ", 6) == 0) {
            char *p = line + 6;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int rows = atoi(p2);
                    int cols = atoi(s2 + 1);
                    if (rows > 0 && cols > 0) {
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(Table));
                        }
                        arr[count].name = strdup(name);
                        arr[count].rows = rows;
                        arr[count].cols = cols;
                        arr[count].cells = malloc(rows * sizeof(char **));
                        for (int i = 0; i < rows; i++) {
                            arr[count].cells[i] = malloc(cols * sizeof(char *));
                            for (int j = 0; j < cols; j++) {
                                arr[count].cells[i][j] = strdup("");
                            }
                        }
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int r = atoi(p2);
                    char *p3 = s2 + 1;
                    char *s3 = strchr(p3, ' ');
                    if (s3) {
                        *s3 = '\0';
                        int c = atoi(p3);
                        char *val = s3 + 1;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(arr[i].name, name) == 0) {
                                if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                                    free(arr[i].cells[r][c]);
                                    arr[i].cells[r][c] = strdup(val);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    int r = atoi(p2);
                    int c = atoi(s2 + 1);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                                if (strlen(arr[i].cells[r][c]) == 0) printf("EMPTY\n");
                                else printf("%s\n", arr[i].cells[r][c]);
                            } else {
                                printf("EMPTY\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (int r = 0; r < arr[i].rows; r++) {
                        for (int c = 0; c < arr[i].cols; c++) {
                            free(arr[i].cells[r][c]);
                        }
                        free(arr[i].cells[r]);
                    }
                    free(arr[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(line, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
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
