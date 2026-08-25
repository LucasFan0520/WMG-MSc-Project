// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

int main() {
    char **lines = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            int idx;
            if (sscanf(line + 7, "%d", &idx) == 1) {
                char *p = line + 7;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                if (idx >= 0 && idx <= count) {
                    lines = realloc(lines, (count + 1) * sizeof(char *));
                    for (int i = count; i > idx; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[idx] = strdup(p);
                    count++;
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            lines = realloc(lines, (count + 1) * sizeof(char *));
            lines[count] = strdup(text);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int i = idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
                lines = realloc(lines, count * sizeof(char *));
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            int idx;
            if (sscanf(line + 6, "%d", &idx) == 1) {
                char *p = line + 6;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    lines[idx] = strdup(p);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *l = lines[i];
                for (int j = 0; l[j] != '\0'; j++) {
                    if (l[j] == ' ') printf("_");
                    else printf("%c", l[j]);
                }
                printf("\n");
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
