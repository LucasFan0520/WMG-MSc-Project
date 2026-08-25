// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

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
    Reminder *list = NULL;
    int count = 0;
    int next_id = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("(%d) %s: %s\n", list[i].priority, list[i].title, list[i].note);
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
                int pri = atoi(rest);
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *title = rest2;
                    char *note = p3 + 1;
                    list = realloc(list, (count + 1) * sizeof(Reminder));
                    list[count].priority = pri;
                    list[count].title = strdup(title);
                    list[count].note = strdup(note);
                    list[count].id = next_id++;
                    count++;
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *title = rest;
                int delta = atoi(p2 + 1);
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].title, title) == 0) {
                        list[i].priority += delta;
                        break;
                    }
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
                    list = realloc(list, count * sizeof(Reminder));
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(rest);
            if (n > 0 && count > 0) {
                Reminder **sorted = malloc(count * sizeof(Reminder *));
                for (int i = 0; i < count; i++) {
                    sorted[i] = &list[i];
                }
                for (int i = 0; i < count - 1; i++) {
                    for (int j = 0; j < count - i - 1; j++) {
                        if (sorted[j]->priority < sorted[j + 1]->priority || 
                            (sorted[j]->priority == sorted[j + 1]->priority && sorted[j]->id > sorted[j + 1]->id)) {
                            Reminder *temp = sorted[j];
                            sorted[j] = sorted[j + 1];
                            sorted[j + 1] = temp;
                        }
                    }
                }
                int limit = (n < count) ? n : count;
                for (int i = 0; i < limit; i++) {
                    printf("(%d) %s: %s\n", sorted[i]->priority, sorted[i]->title, sorted[i]->note);
                }
                free(sorted);
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