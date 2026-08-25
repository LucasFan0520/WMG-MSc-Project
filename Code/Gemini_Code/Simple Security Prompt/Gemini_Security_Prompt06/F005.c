// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    char **list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    for (int j = 0; list[i][j] != '\0'; j++) {
                        if (list[i][j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(list[i][j]);
                        }
                    }
                    putchar('\n');
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "APPEND") == 0) {
            list = realloc(list, (count + 1) * sizeof(char *));
            list[count] = strdup(rest);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                int idx = atoi(rest);
                char *text = p2 + 1;
                if (idx >= 0 && idx <= count) {
                    list = realloc(list, (count + 1) * sizeof(char *));
                    for (int i = count; i > idx; i--) {
                        list[i] = list[i - 1];
                    }
                    list[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = atoi(rest);
            if (idx >= 0 && idx < count) {
                free(list[idx]);
                for (int i = idx; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(char *));
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                int idx = atoi(rest);
                char *text = p2 + 1;
                if (idx >= 0 && idx < count) {
                    free(list[idx]);
                    list[idx] = strdup(text);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i]);
    }
    free(list);
    return 0;
}