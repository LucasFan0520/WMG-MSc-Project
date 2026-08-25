// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

int main() {
    char **lines = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    for (int j = 0; lines[i][j]; j++) {
                        if (lines[i][j] == ' ') printf("_");
                        else printf("%c", lines[i][j]);
                    }
                    printf("\n");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "APPEND") == 0) {
            lines = realloc(lines, sizeof(char*) * (count + 1));
            lines[count] = strdup(args);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int idx = atoi(args);
                char *text = p2 + 1;
                if (idx >= 0 && idx <= count) {
                    lines = realloc(lines, sizeof(char*) * (count + 1));
                    for (int j = count; j > idx; j--) {
                        lines[j] = lines[j - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = atoi(args);
            if (idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int j = idx; j < count - 1; j++) {
                    lines[j] = lines[j + 1];
                }
                count--;
                if (count == 0) { free(lines); lines = NULL; }
                else { lines = realloc(lines, sizeof(char*) * count); }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                int idx = atoi(args);
                char *text = p2 + 1;
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    lines[idx] = strdup(text);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
