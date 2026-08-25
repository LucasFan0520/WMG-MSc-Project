// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

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
    Draft *list = NULL;
    int count = 0;
    char *h_title = NULL;
    char *h_body = NULL;
    int h_avail = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "RESTORE") == 0) {
                if (h_avail) {
                    list = realloc(list, (count + 1) * sizeof(Draft));
                    list[count].title = strdup(h_title);
                    list[count].body = strdup(h_body);
                    count++;
                    free(h_title);
                    free(h_body);
                    h_title = NULL;
                    h_body = NULL;
                    h_avail = 0;
                }
            } else if (strcmp(line, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", list[i].title, list[i].body);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = rest;
                char *body = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "CREATE") == 0 && found == -1) {
                    list = realloc(list, (count + 1) * sizeof(Draft));
                    list[count].title = strdup(title);
                    list[count].body = strdup(body);
                    count++;
                } else if (strcmp(cmd, "REVISE") == 0 && found != -1) {
                    free(list[found].body);
                    list[found].body = strdup(body);
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                if (h_avail) {
                    free(h_title);
                    free(h_body);
                }
                h_title = strdup(list[found].title);
                h_body = strdup(list[found].body);
                h_avail = 1;
                free(list[found].title);
                free(list[found].body);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Draft));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s %s\n", list[found].title, list[found].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].body);
    }
    free(list);
    if (h_avail) {
        free(h_title);
        free(h_body);
    }
    return 0;
}