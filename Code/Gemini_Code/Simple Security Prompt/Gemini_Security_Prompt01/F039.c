// F039.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int rows;
    int cols;
    char ***cells;
} CountedTable;

int main() {
    CountedTable *tables = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "TABLE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int r = atoi(space2 + 1);
                char *space3 = strchr(space2 + 1, ' ');
                if (space3) {
                    int c = atoi(space3 + 1);
                    if (r > 0 && c > 0) {
                        tables = realloc(tables, (count + 1) * sizeof(CountedTable));
                        if (!tables) return 1;
                        tables[count].name = strdup(name);
                        tables[count].rows = r;
                        tables[count].cols = c;
                        tables[count].cells = malloc(r * sizeof(char **));
                        if (!tables[count].cells) return 1;
                        for (int i = 0; i < r; i++) {
                            tables[count].cells[i] = malloc(c * sizeof(char *));
                            if (!tables[count].cells[i]) return 1;
                            for (int j = 0; j < c; j++) {
                                tables[count].cells[i][j] = strdup("EMPTY");
                            }
                        }
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int r = atoi(space2 + 1);
                char *space3 = strchr(space2 + 1, ' ');
                if (space3) {
                    *space3 = '\0';
                    int c = atoi(space3 + 1);
                    char *val = space3 + 1;
                    while (*val == ' ') val++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(tables[i].name, name) == 0) {
                            if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                                free(tables[i].cells[r][c]);
                                tables[i].cells[r][c] = strdup(val);
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int r = atoi(space2 + 1);
                char *space3 = strchr(space2 + 1, ' ');
                if (space3) {
                    int c = atoi(space3 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(tables[i].name, name) == 0) {
                            if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                                printf("%s\n", tables[i].cells[r][c]);
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(tables[i].name, args) == 0) {
                    free(tables[i].name);
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) {
                            free(tables[i].cells[r][c]);
                        }
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    for (int j = i; j < count - 1; j++) {
                        tables[j] = tables[j + 1];
                    }
                    count--;
                    tables = realloc(tables, count * sizeof(CountedTable));
                    if (count > 0 && !tables) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(tables[i].name);
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
    }
    free(tables);
    return 0;
}
