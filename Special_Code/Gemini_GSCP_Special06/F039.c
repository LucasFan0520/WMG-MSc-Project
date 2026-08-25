// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
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
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int rows = atoi(r_s);
            int cols = atoi(c_s);
            if (rows > 0 && cols > 0) {
                char ***cells = malloc(rows * sizeof(char **));
                if (cells) {
                    for (int i = 0; i < rows; i++) {
                        cells[i] = calloc(cols, sizeof(char *));
                    }
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Table *narr = realloc(arr, cap * sizeof(Table));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].name = mystrdup(name);
                        arr[count].rows = rows;
                        arr[count].cols = cols;
                        arr[count].cells = cells;
                        count++;
                    } else {
                        for (int i = 0; i < rows; i++) free(cells[i]);
                        free(cells);
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            size_t vl = strlen(value);
            while (vl > 0 && (value[vl-1] == '\r' || value[vl-1] == '\n')) {
                value[vl-1] = '\0';
                vl--;
            }
            int r = atoi(r_s);
            int c = atoi(c_s);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                        free(arr[i].cells[r][c]);
                        arr[i].cells[r][c] = mystrdup(value);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int r = atoi(r_s);
            int c = atoi(c_s);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (r >= 0 && r < arr[i].rows && c >= 0 && c < arr[i].cols) {
                        if (arr[i].cells[r][c]) printf("%s\n", arr[i].cells[r][c]);
                        else printf("EMPTY\n");
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
