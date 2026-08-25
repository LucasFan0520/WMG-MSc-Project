// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main(void) {
    RouteStop *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                RouteStop *nb = realloc(list, cap * sizeof(RouteStop));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].stop = strdup(stop);
                list[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    RouteStop *nb = realloc(list, cap * sizeof(RouteStop));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    for (size_t j = count; j > (size_t)idx; j--) {
                        list[j] = list[j - 1];
                    }
                    list[idx].stop = strdup(stop);
                    list[idx].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int fromIdx = atoi(from_str);
            int toIdx = atoi(p);
            if (fromIdx >= 0 && (size_t)fromIdx < count && toIdx >= 0 && (size_t)toIdx < count) {
                RouteStop temp = list[fromIdx];
                if (fromIdx < toIdx) {
                    for (size_t j = (size_t)fromIdx; j < (size_t)toIdx; j++) {
                        list[j] = list[j + 1];
                    }
                } else {
                    for (size_t j = (size_t)fromIdx; j > (size_t)toIdx; j--) {
                        list[j] = list[j - 1];
                    }
                }
                list[toIdx] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx = atoi(arg1);
            if (idx >= 0 && (size_t)idx < count) {
                free(list[idx].stop);
                free(list[idx].note);
                for (size_t j = (size_t)idx; j < count - 1; j++) {
                    list[j] = list[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].stop, arg1) == 0) {
                    printf("%s\n", list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", list[i].stop, list[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].stop);
        free(list[i].note);
    }
    free(list);
    return 0;
}
