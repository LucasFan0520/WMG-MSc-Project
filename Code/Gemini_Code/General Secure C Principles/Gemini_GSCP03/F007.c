// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

int main() {
    Activity *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(items[i].time);
                free(items[i].label);
                free(items[i].note);
            }
            count = 0;
            free(line);
            continue;
        }
        if (strcmp(cmd, "SORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = i + 1; j < count; j++) {
                    int cmp = strcmp(items[i].time, items[j].time);
                    if (cmp > 0 || (cmp == 0 && strcmp(items[i].label, items[j].label) > 0)) {
                        Activity tmp = items[i];
                        items[i] = items[j];
                        items[j] = tmp;
                    }
                }
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", items[i].time, items[i].label, items[i].note);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *time = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Activity *nb = realloc(items, cap * sizeof(Activity));
                if (nb) items = nb;
            }
            items[count].time = strdup(time);
            items[count].label = strdup(label);
            items[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].time, time) == 0 && strcmp(items[i].label, label) == 0) {
                    free(items[i].time);
                    free(items[i].label);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            int idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].label, label) == 0) {
                    if (idx == -1 || strcmp(items[i].time, items[idx].time) < 0) {
                        idx = (int)i;
                    }
                }
            }
            if (idx != -1) {
                printf("%s %s %s\n", items[idx].time, items[idx].label, items[idx].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].time);
        free(items[i].label);
        free(items[i].note);
    }
    free(items);
    return 0;
}
