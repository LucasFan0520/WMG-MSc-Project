// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *stop;
    char *note;
} StopInfo;

int main(void) {
    StopInfo *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
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
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                StopInfo *nitems = realloc(items, cap * sizeof(StopInfo));
                if (nitems) items = nitems;
            }
            items[count].stop = mystrdup(stop);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    StopInfo *nitems = realloc(items, cap * sizeof(StopInfo));
                    if (nitems) items = nitems;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    items[i] = items[i-1];
                }
                items[idx].stop = mystrdup(stop);
                items[idx].note = mystrdup(note);
                count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *f_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *t_s = p;
            size_t tlen = strlen(t_s);
            while (tlen > 0 && (t_s[tlen-1] == '\n' || t_s[tlen-1] == '\n')) {
                t_s[tlen-1] = '\0';
                tlen--;
            }
            int fromIdx = atoi(f_s);
            int toIdx = atoi(t_s);
            if (fromIdx >= 0 && (size_t)fromIdx < count && toIdx >= 0 && (size_t)toIdx < count && fromIdx != toIdx) {
                StopInfo target = items[fromIdx];
                if (fromIdx < toIdx) {
                    for (size_t i = (size_t)fromIdx; i < (size_t)toIdx; i++) {
                        items[i] = items[i+1];
                    }
                } else {
                    for (size_t i = (size_t)fromIdx; i > (size_t)toIdx; i--) {
                        items[i] = items[i-1];
                    }
                }
                items[toIdx] = target;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_s = p;
            size_t ilen = strlen(idx_s);
            while (ilen > 0 && (idx_s[ilen-1] == '\n' || idx_s[ilen-1] == '\n')) {
                idx_s[ilen-1] = '\0';
                ilen--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && (size_t)idx < count) {
                free(items[idx].stop);
                free(items[idx].note);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    items[i] = items[i+1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = p;
            size_t slen = strlen(stop);
            while (slen > 0 && (stop[slen-1] == '\n' || stop[slen-1] == '\n')) {
                stop[slen-1] = '\0';
                slen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].stop, stop) == 0) {
                    printf("%s\n", items[i].note);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%zu: %s (%s)\n", i, items[i].stop, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].stop);
        free(items[i].note);
    }
    free(items);
    return 0;
}
