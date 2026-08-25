// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

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
    Activity *list = NULL;
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
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Activity *nl = realloc(list, capacity * sizeof(Activity));
                if (!nl) break;
                list = nl;
            }
            list[count].time = strdup(time);
            list[count].label = strdup(label);
            list[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *time = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].time, time) == 0 && strcmp(list[i].label, label) == 0) {
                    free(list[i].time);
                    free(list[i].label);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int best_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].label, label) == 0) {
                    if (best_idx == -1 || strcmp(list[i].time, list[best_idx].time) < 0) {
                        best_idx = (int)i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%s %s %s\n", list[best_idx].time, list[best_idx].label, list[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = i + 1; j < count; j++) {
                    int cmp = strcmp(list[i].time, list[j].time);
                    if (cmp > 0 || (cmp == 0 && strcmp(list[i].label, list[j].label) > 0)) {
                        Activity tmp = list[i];
                        list[i] = list[j];
                        list[j] = tmp;
                    }
                }
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", list[i].time, list[i].label, list[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(list[i].time);
                free(list[i].label);
                free(list[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].time);
        free(list[i].label);
        free(list[i].note);
    }
    free(list);
    return 0;
}
