// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

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
    Note *list = NULL;
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
                    printf("[%s] %s\n", list[i].category, list[i].text);
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
                char *cat = rest;
                char *text = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Note));
                list[count].category = strdup(cat);
                list[count].text = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = rest;
            int idx = 0;
            while (idx < count) {
                if (strcmp(list[idx].category, cat) == 0) {
                    free(list[idx].category);
                    free(list[idx].text);
                    for (int j = idx; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            if (count == 0) {
                free(list);
                list = NULL;
            } else {
                list = realloc(list, count * sizeof(Note));
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = rest;
            int idx = 0;
            while (idx < count) {
                if (strcmp(list[idx].category, cat) != 0) {
                    free(list[idx].category);
                    free(list[idx].text);
                    for (int j = idx; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            if (count == 0) {
                free(list);
                list = NULL;
            } else {
                list = realloc(list, count * sizeof(Note));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = rest;
            for (int i = 0; i < count; i++) {
                if (strstr(list[i].text, word) != NULL) {
                    printf("[%s] %s\n", list[i].category, list[i].text);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].category);
        free(list[i].text);
    }
    free(list);
    return 0;
}