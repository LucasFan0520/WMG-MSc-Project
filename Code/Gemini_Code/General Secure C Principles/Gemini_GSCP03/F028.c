// F028.c
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
    char *stop;
    char *note;
} Stop;

int main() {
    Stop *items = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].stop, items[i].note);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Stop *nb = realloc(items, cap * sizeof(Stop));
                if (nb) items = nb;
            }
            items[count].stop = strdup(stop);
            items[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    Stop *nb = realloc(items, cap * sizeof(Stop));
                    if (nb) items = nb;
                }
                for (int j = count; j > idx; j--) {
                    items[j] = items[j - 1];
                }
                items[idx].stop = strdup(stop);
                items[idx].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *to_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int from = atoi(from_str);
            int to = atoi(to_str);
            if (from >= 0 && from < count && to >= 0 && to < count && from != to) {
                Stop tmp = items[from];
                if (from < to) {
                    for (int j = from; j < to; j++) {
                        items[j] = items[j + 1];
                    }
                } else {
                    for (int j = from; j > to; j--) {
                        items[j] = items[j - 1];
                    }
                }
                items[to] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx = atoi(idx_str);
            if (idx >= 0 && idx < count) {
                free(items[idx].stop);
                free(items[idx].note);
                for (int j = idx; j < count - 1; j++) {
                    items[j] = items[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].stop, stop) == 0) {
                    printf("%s\n", items[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].stop);
        free(items[i].note);
    }
    free(items);
    return 0;
}
