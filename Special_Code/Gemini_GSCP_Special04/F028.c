/* F028.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} Stop;

int main(void) {
    Stop *route = NULL;
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Stop *nr = realloc(route, cap * sizeof(Stop));
                if (nr) route = nr;
            }
            char *nname = mystrdup(stop);
            char *nnote = mystrdup(note);
            if (nname && nnote) {
                route[count].name = nname;
                route[count].note = nnote;
                count++;
            } else {
                free(nname); free(nnote);
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            long long idx = strtoll(idx_str, NULL, 10);
            if (idx >= 0 && idx <= (long long)count) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Stop *nr = realloc(route, cap * sizeof(Stop));
                    if (nr) route = nr;
                }
                char *nname = mystrdup(stop);
                char *nnote = mystrdup(note);
                if (nname && nnote) {
                    for (size_t i = count; i > (size_t)idx; i--) {
                        route[i] = route[i - 1];
                    }
                    route[idx].name = nname;
                    route[idx].note = nnote;
                    count++;
                } else {
                    free(nname); free(nnote);
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            while (*p == ' ') p++;
            char *from_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *to_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long fromIdx = strtoll(from_str, NULL, 10);
            long long toIdx = strtoll(to_str, NULL, 10);
            if (fromIdx >= 0 && fromIdx < (long long)count && toIdx >= 0 && toIdx < (long long)count) {
                Stop temp = route[fromIdx];
                if (fromIdx < toIdx) {
                    for (size_t i = (size_t)fromIdx; i < (size_t)toIdx; i++) {
                        route[i] = route[i + 1];
                    }
                } else {
                    for (size_t i = (size_t)fromIdx; i > (size_t)toIdx; i--) {
                        route[i] = route[i - 1];
                    }
                }
                route[toIdx] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *idx_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long idx = strtoll(idx_str, NULL, 10);
            if (idx >= 0 && idx < (long long)count) {
                free(route[idx].name);
                free(route[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    route[i] = route[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(route[i].name, stop) == 0) {
                    printf("%s\n", route[i].note);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s - %s\n", route[i].name, route[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(route[i].name);
        free(route[i].note);
    }
    free(route);
    return 0;
}
