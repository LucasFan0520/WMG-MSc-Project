// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

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
    Task *list = NULL;
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
            if (strcmp(line, "REPORT") == 0) {
                printf("TODO:\n");
                for (int i = 0; i < count; i++) {
                    if (list[i].status == 0) printf("%s %s\n", list[i].title, list[i].note);
                }
                printf("DOING:\n");
                for (int i = 0; i < count; i++) {
                    if (list[i].status == 1) printf("%s %s\n", list[i].title, list[i].note);
                }
                printf("DONE:\n");
                for (int i = 0; i < count; i++) {
                    if (list[i].status == 2) printf("%s %s\n", list[i].title, list[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "TODO") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = rest;
                char *note = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Task));
                list[count].title = strdup(title);
                list[count].note = strdup(note);
                list[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0 || strcmp(cmd, "DONE") == 0) {
            char *title = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    if (strcmp(cmd, "START") == 0 && list[i].status == 0) {
                        list[i].status = 1;
                    } else if (strcmp(cmd, "DONE") == 0 && list[i].status == 1) {
                        list[i].status = 2;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].title);
                free(list[found].note);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Task));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    const char *st = (list[i].status == 0) ? "TODO" : ((list[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st, list[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].note);
    }
    free(list);
    return 0;
}