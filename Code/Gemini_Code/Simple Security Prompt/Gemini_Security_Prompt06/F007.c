// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

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
    Activity *list = NULL;
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
            if (strcmp(line, "SORT") == 0) {
                for (int i = 0; i < count - 1; i++) {
                    for (int j = 0; j < count - i - 1; j++) {
                        int cmp = strcmp(list[j].time, list[j + 1].time);
                        if (cmp > 0 || (cmp == 0 && strcmp(list[j].label, list[j + 1].label) > 0)) {
                            Activity temp = list[j];
                            list[j] = list[j + 1];
                            list[j + 1] = temp;
                        }
                    }
                }
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s\n", list[i].time, list[i].label, list[i].note);
                }
            } else if (strcmp(line, "CLEAR") == 0) {
                for (int i = 0; i < count; i++) {
                    free(list[i].time);
                    free(list[i].label);
                    free(list[i].note);
                }
                free(list);
                list = NULL;
                count = 0;
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
                char *time = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *label = rest2;
                    char *note = p3 + 1;
                    list = realloc(list, (count + 1) * sizeof(Activity));
                    list[count].time = strdup(time);
                    list[count].label = strdup(label);
                    list[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *time = rest;
                char *label = p2 + 1;
                int i = 0;
                while (i < count) {
                    if (strcmp(list[i].time, time) == 0 && strcmp(list[i].label, label) == 0) {
                        free(list[i].time);
                        free(list[i].label);
                        free(list[i].note);
                        for (int j = i; j < count - 1; j++) {
                            list[j] = list[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Activity));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = rest;
            int best_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].label, label) == 0) {
                    if (best_idx == -1 || strcmp(list[i].time, list[best_idx].time) < 0) {
                        best_idx = i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%s %s %s\n", list[best_idx].time, list[best_idx].label, list[best_idx].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].time);
        free(list[i].label);
        free(list[i].note);
    }
    free(list);
    return 0;
}