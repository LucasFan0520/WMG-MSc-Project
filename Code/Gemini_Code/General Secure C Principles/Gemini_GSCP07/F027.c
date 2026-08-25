// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Reminder *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *prio_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Reminder *nl = realloc(list, capacity * sizeof(Reminder));
                if (!nl) break;
                list = nl;
            }
            list[count].priority = atoi(prio_str);
            list[count].title = strdup(title);
            list[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *delta_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int delta = atoi(delta_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    list[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    free(list[i].title);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p == ' ') p++;
            char *n_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int n = atoi(n_str);
            if (n > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    memcpy(temp, list, count * sizeof(Reminder));
                    for (size_t i = 0; i < count; i++) {
                        for (size_t j = i + 1; j < count; j++) {
                            if (temp[i].priority < temp[j].priority) {
                                Reminder t = temp[i];
                                temp[i] = temp[j];
                                temp[j] = t;
                            }
                        }
                    }
                    int limit = n < (int)count ? n : (int)count;
                    for (int i = 0; i < limit; i++) {
                        printf("(%d) %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", list[i].priority, list[i].title, list[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].note);
    }
    free(list);
    return 0;
}
