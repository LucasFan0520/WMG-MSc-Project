// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char ***cells;
    size_t rows;
    size_t cols;
} Table;

int main(void) {
    Table *tables = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long rows = atoll(r_s);
            long long cols = atoll(c_s);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0' && rows > 0 && cols > 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Table *new_tables = realloc(tables, capacity * sizeof(Table));
                    if (new_tables) tables = new_tables;
                }
                if (count < capacity) {
                    tables[count].name = strdup(name);
                    tables[count].rows = (size_t)rows;
                    tables[count].cols = (size_t)cols;
                    tables[count].cells = malloc(rows * sizeof(char **));
                    if (tables[count].cells) {
                        for (long long i = 0; i < rows; i++) {
                            tables[count].cells[i] = calloc(cols, sizeof(char *));
                        }
                    }
                    count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;
            long long r = atoll(r_s);
            long long c = atoll(c_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && (size_t)r < tables[i].rows && c >= 0 && (size_t)c < tables[i].cols) {
                        if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                        tables[i].cells[r][c] = strdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *r_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *c_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long r = atoll(r_s);
            long long c = atoll(c_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && (size_t)r < tables[i].rows && c >= 0 && (size_t)c < tables[i].cols) {
                        if (tables[i].cells[r][c]) {
                            printf("%s\n", tables[i].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (size_t r = 0; r < tables[i].rows; r++) {
                        for (size_t c = 0; c < tables[i].cols; c++) {
                            if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
                        }
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    for (size_t j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(tables[i].name);
        for (size_t r = 0; r < tables[i].rows; r++) {
            for (size_t c = 0; c < tables[i].cols; c++) {
                if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
