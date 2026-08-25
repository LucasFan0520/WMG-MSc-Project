// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

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
    Stop *list = NULL;
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
                    printf("%d: %s - %s\n", i, list[i].stop, list[i].note);
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
                char *stop = rest;
                char *note = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(Stop));
                list[count].stop = strdup(stop);
                list[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                int idx = atoi(rest);
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *stop = rest2;
                    char *note = p3 + 1;
                    if (idx >= 0 && idx <= count) {
                        list = realloc(list, (count + 1) * sizeof(Stop));
                        for (int i = count; i > idx; i--) {
                            list[i] = list[i - 1];
                        }
                        list[idx].stop = strdup(stop);
                        list[idx].note = strdup(note);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                int fromIdx = atoi(rest);
                int toIdx = atoi(p2 + 1);
                if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count && fromIdx != toIdx) {
                    Stop temp = list[fromIdx];
                    if (fromIdx < toIdx) {
                        for (int i = fromIdx; i < toIdx; i++) {
                            list[i] = list[i + 1];
                        }
                    } else {
                        for (int i = fromIdx; i > toIdx; i--) {
                            list[i] = list[i - 1];
                        }
                    }
                    list[toIdx] = temp;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = atoi(rest);
            if (idx >= 0 && idx < count) {
                free(list[idx].stop);
                free(list[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Stop));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].stop, stop) == 0) {
                    printf("%s\n", list[i].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].stop);
        free(list[i].note);
    }
    free(list);
    return 0;
}