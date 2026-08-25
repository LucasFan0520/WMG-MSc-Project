// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char ***cells;
    int rows;
    int cols;
} CountedTable;

int main() {
    CountedTable *tables = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "TABLE") == 0) {
            char name[256];
            int r = 0, c = 0;
            if (sscanf(ptr, "%255s %d %d", name, &r, &c) == 3 && r > 0 && c > 0) {
                tables = realloc(tables, (count + 1) * sizeof(CountedTable));
                tables[count].name = strdup(name);
                tables[count].rows = r;
                tables[count].cols = c;
                tables[count].cells = malloc(r * sizeof(char **));
                for (int i = 0; i < r; i++) {
                    tables[count].cells[i] = malloc(c * sizeof(char *));
                    for (int j = 0; j < c; j++) {
                        tables[count].cells[i][j] = strdup("EMPTY");
                    }
                }
                count++;
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char name[256], val[256];
            int r = -1, c = -1;
            if (sscanf(ptr, "%255s %d %d %255s", name, &r, &c, val) == 4) {
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
        } else if (strcmp(cmd, "GET") == 0) {
            char name[256];
            int r = -1, c = -1;
            if (sscanf(ptr, "%255s %d %d", name, &r, &c) == 3) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    if (r >= 0 && r < tables[found].rows && c >= 0 && c < tables[found].cols) {
                        printf("%s\n", tables[found].cells[r][c]);
                    } else {
                        printf("INVALID\n");
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(tables[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(tables[found].name);
                    for (int i = 0; i < tables[found].rows; i++) {
                        for (int j = 0; j < tables[found].cols; j++) {
                            free(tables[found].cells[i][j]);
                        }
                        free(tables[found].cells[i]);
                    }
                    free(tables[found].cells);
                    for (int i = found; i < count - 1; i++) {
                        tables[i] = tables[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        tables = realloc(tables, count * sizeof(CountedTable));
                    } else {
                        free(tables);
                        tables = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    for (int k = 0; k < count; k++) {
        free(tables[k].name);
        for (int i = 0; i < tables[k].rows; i++) {
            for (int j = 0; j < tables[k].cols; j++) {
                free(tables[k].cells[i][j]);
            }
            free(tables[k].cells[i]);
        }
        free(tables[k].cells);
    }
    free(tables);
    return 0;
}
