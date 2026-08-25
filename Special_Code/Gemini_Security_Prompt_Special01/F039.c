// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} CountedTable;

int main(void) {
    CountedTable *tables = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *r_s = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                r_s++;
            }
            while (*r_s == ' ') r_s++;
            char *r_end = r_s;
            while (*r_end && *r_end != ' ') r_end++;
            char *c_s = r_end;
            if (*r_end != '\0') {
                *r_end = '\0';
                c_s++;
            }
            while (*c_s == ' ') c_s++;
            char *c_end = c_s;
            while (*c_end && *c_end != ' ') c_end++;
            *c_end = '\0';
            int rows = atoi(r_s);
            int cols = atoi(c_s);
            if (rows > 0 && cols > 0) {
                char ***cells = malloc(rows * sizeof(char **));
                if (cells) {
                    for (int i = 0; i < rows; i++) {
                        cells[i] = malloc(cols * sizeof(char *));
                        for (int j = 0; j < cols; j++) {
                            cells[i][j] = NULL;
                        }
                    }
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        CountedTable *new_t = realloc(tables, capacity * sizeof(CountedTable));
                        if (new_t) tables = new_t;
                    }
                    tables[count].name = mystrdup(name);
                    tables[count].cells = cells;
                    tables[count].rows = rows;
                    tables[count].cols = cols;
                    count++;
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *r_s = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                r_s++;
            }
            while (*r_s == ' ') r_s++;
            char *r_end = r_s;
            while (*r_end && *r_end != ' ') r_end++;
            char *c_s = r_end;
            if (*r_end != '\0') {
                *r_end = '\0';
                c_s++;
            }
            while (*c_s == ' ') c_s++;
            char *c_end = c_s;
            while (*c_end && *c_end != ' ') c_end++;
            char *val = c_end;
            if (*c_end != '\0') {
                *c_end = '\0';
                val++;
            }
            while (*val == ' ') val++;
            int r_idx = atoi(r_s);
            int c_idx = atoi(c_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < tables[i].rows && c_idx >= 0 && c_idx < tables[i].cols) {
                        if (tables[i].cells[r_idx][c_idx]) free(tables[i].cells[r_idx][c_idx]);
                        tables[i].cells[r_idx][c_idx] = mystrdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *r_s = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                r_s++;
            }
            while (*r_s == ' ') r_s++;
            char *r_end = r_s;
            while (*r_end && *r_end != ' ') r_end++;
            char *c_s = r_end;
            if (*r_end != '\0') {
                *r_end = '\0';
                c_s++;
            }
            while (*c_s == ' ') c_s++;
            char *c_end = c_s;
            while (*c_end && *c_end != ' ') c_end++;
            *c_end = '\0';
            int r_idx = atoi(r_s);
            int c_idx = atoi(c_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r_idx >= 0 && r_idx < tables[i].rows && c_idx >= 0 && c_idx < tables[i].cols) {
                        if (tables[i].cells[r_idx][c_idx]) {
                            printf("%s\n", tables[i].cells[r_idx][c_idx]);
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
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    free(tables[i].name);
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) {
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
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                if (tables[i].cells[r][c]) free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
