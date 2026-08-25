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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *stop;
    char *note;
} RouteStop;

int main() {
    RouteStop *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                RouteStop *narr = realloc(arr, cap * sizeof(RouteStop));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].stop = mystrdup(stop);
                arr[count].note = mystrdup(note);
                count++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    RouteStop *narr = realloc(arr, cap * sizeof(RouteStop));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    for (int j = count; j > idx; j--) {
                        arr[j] = arr[j - 1];
                    }
                    arr[idx].stop = mystrdup(stop);
                    arr[idx].note = mystrdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *from_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *to_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int fromIdx = atoi(from_s);
            int toIdx = atoi(to_s);
            if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count && fromIdx != toIdx) {
                RouteStop tmp = arr[fromIdx];
                if (fromIdx < toIdx) {
                    for (int j = fromIdx; j < toIdx; j++) {
                        arr[j] = arr[j + 1];
                    }
                } else {
                    for (int j = fromIdx; j > toIdx; j--) {
                        arr[j] = arr[j - 1];
                    }
                }
                arr[toIdx] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int idx = atoi(idx_s);
            if (idx >= 0 && idx < count) {
                free(arr[idx].stop);
                free(arr[idx].note);
                for (int j = idx; j < count - 1; j++) {
                    arr[j] = arr[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *stop = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].stop, stop) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d: %s - %s\n", i, arr[i].stop, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].stop);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
