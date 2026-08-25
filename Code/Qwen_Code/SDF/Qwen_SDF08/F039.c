// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size ? size : 1);
    if (!ptr)
        exit(EXIT_FAILURE);
    return ptr;
}
static void *safe_realloc(void *ptr, size_t size) {
    void *temp = realloc(ptr, size ? size : 1);
    if (!temp)
        exit(EXIT_FAILURE);
    return temp;
}
static char *safe_strdup(const char *s) {
    if (!s)
        return NULL;
    char *ptr = strdup(s);
    if (!ptr)
        exit(EXIT_FAILURE);
    return ptr;
}
static char *read_line(FILE *stream) {
    size_t capacity = 256, length = 0;
    char *buffer = safe_malloc(capacity);
    while (fgets(buffer + length, capacity - length, stream)) {
        length += strlen(buffer + length);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
            return buffer;
        }
        if (length == capacity - 1) {
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return NULL;
            }
            capacity *= 2;
            buffer = safe_realloc(buffer, capacity);
        }
    }
    if (length > 0)
        return buffer;
    free(buffer);
    return NULL;
}

typedef struct {
    char ***cells;
    int rows;
    int cols;
} Table;
typedef struct {
    char *name;
    Table *tbl;
} TblRec;
typedef struct {
    TblRec *items;
    size_t size;
    size_t capacity;
} TList;

int main(void) {
    TList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "TABLE") == 0) {
            char *p = line + 5;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int rows = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int cols = atoi(p);
            if (rows > 0 && cols > 0) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(TblRec));
                }
                list.items[list.size].name = safe_strdup(name);
                Table *t = safe_malloc(sizeof(Table));
                t->rows = rows;
                t->cols = cols;
                t->cells = safe_malloc(rows * sizeof(char **));
                for (int i = 0; i < rows; i++) {
                    t->cells[i] = safe_malloc(cols * sizeof(char *));
                    for (int j = 0; j < cols; j++)
                        t->cells[i][j] = NULL;
                }
                list.items[list.size].tbl = t;
                list.size++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int row = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int col = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    Table *t = list.items[i].tbl;
                    if (row >= 0 && row < t->rows && col >= 0 && col < t->cols) {
                        free(t->cells[row][col]);
                        t->cells[row][col] = safe_strdup(p);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int row = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int col = atoi(p);
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    Table *t = list.items[i].tbl;
                    if (row >= 0 && row < t->rows && col >= 0 && col < t->cols) {
                        if (t->cells[row][col])
                            printf("%s\n", t->cells[row][col]);
                        else
                            printf("EMPTY\n");
                    } else
                        printf("EMPTY\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    free(list.items[i].name);
                    Table *t = list.items[i].tbl;
                    for (int r = 0; r < t->rows; r++) {
                        for (int c = 0; c < t->cols; c++)
                            free(t->cells[r][c]);
                        free(t->cells[r]);
                    }
                    free(t->cells);
                    free(t);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%s\n", list.items[i].name);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        Table *t = list.items[i].tbl;
        for (int r = 0; r < t->rows; r++) {
            for (int c = 0; c < t->cols; c++)
                free(t->cells[r][c]);
            free(t->cells[r]);
        }
        free(t->cells);
        free(t);
    }
    free(list.items);
    return 0;
}