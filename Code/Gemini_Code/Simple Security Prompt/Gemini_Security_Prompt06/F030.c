// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

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
    Rule *list = NULL;
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
            if (strcmp(line, "RULES") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%d: %s -> %s\n", i, list[i].old_str, list[i].new_str);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *old_s = rest;
                char *new_s = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Rule));
                list[count].old_str = strdup(old_s);
                list[count].new_str = strdup(new_s);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = rest;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(text, list[i].old_str) == 0) {
                    printf("%s\n", list[i].new_str);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_s = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].old_str, old_s) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].old_str);
                free(list[found].new_str);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Rule));
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                int i = atoi(rest);
                int j = atoi(p2 + 1);
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    Rule temp = list[i];
                    list[i] = list[j];
                    list[j] = temp;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].old_str);
        free(list[i].new_str);
    }
    free(list);
    return 0;
}