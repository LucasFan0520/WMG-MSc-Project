// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} RouteStop;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    RouteStop *list = NULL;
    int count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            list = realloc(list, (count + 1) * sizeof(RouteStop));
            list[count].stop = strdup(stop);
            list[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ') p++;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            int idx = atoi(idx_s);
            if (idx >= 0 && idx <= count) {
                list = realloc(list, (count + 1) * sizeof(RouteStop));
                for (int i = count; i > idx; i--) {
                    list[i] = list[i - 1];
                }
                list[idx].stop = strdup(stop);
                list[idx].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ') p++;
            char *from_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int fromIdx = atoi(from_s);
            int toIdx = atoi(p);
            if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count && fromIdx != toIdx) {
                RouteStop target = list[fromIdx];
                if (fromIdx < toIdx) {
                    for (int i = fromIdx; i < toIdx; i++) {
                        list[i] = list[i + 1];
                    }
                } else {
                    for (int i = fromIdx; i > toIdx; i--) {
                        list[i] = list[i - 1];
                    }
                }
                list[toIdx] = target;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            int idx = atoi(p);
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
                    list = realloc(list, count * sizeof(RouteStop));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].stop, stop) == 0) {
                    printf("%s\n", list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", list[i].stop, list[i].note);
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
