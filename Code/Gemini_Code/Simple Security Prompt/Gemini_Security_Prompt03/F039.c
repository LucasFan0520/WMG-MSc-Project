// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} Table39;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

int main(void) {
    Table39 *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            int r = atoi(r_s);
            int c = atoi(p);
            if (r > 0 && c > 0) {
                list = realloc(list, (count + 1) * sizeof(Table39));
                list[count].name = strdup(name);
                list[count].rows = r;
                list[count].cols = c;
                list[count].cells = malloc(r * sizeof(char **));
                for (int i = 0; i < r; i++) {
                    list[count].cells[i] = calloc(c, sizeof(char *));
                }
                count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *c_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            int r_idx = atoi(r_s);
            int c_idx = atoi(c_s);
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < list[i].rows && c_idx >= 0 && c_idx < list[i].cols) {
                        if (list[i].cells[r_idx][c_idx]) free(list[i].cells[r_idx][c_idx]);
                        list[i].cells[r_idx][c_idx] = strdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *r_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            int r_idx = atoi(r_s);
            int c_idx = atoi(p);
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < list[i].rows && c_idx >= 0 && c_idx < list[i].cols) {
                        if (list[i].cells[r_idx][c_idx]) {
                            printf("%s\n", list[i].cells[r_idx][c_idx]);
                        } else {
                            printf("EMPTY\n");
                        }
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    for (int r = 0; r < list[i].rows; r++) {
                        for (int c = 0; c < list[i].cols; c++) {
                            if (list[i].cells[r][c]) free(list[i].cells[r][c]);
                        }
                        free(list[i].cells[r]);
                    }
                    free(list[i].cells);
                    list[i].name = NULL;
                    list[i].cells = NULL;
                    list[i].rows = 0;
                    list[i].cols = 0;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s\n", list[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            for (int r = 0; r < list[i].rows; r++) {
                for (int c = 0; c < list[i].cols; c++) {
                    if (list[i].cells[r][c]) free(list[i].cells[r][c]);
                }
                free(list[i].cells[r]);
            }
            free(list[i].cells);
        }
    }
    free(list);
    return 0;
}
