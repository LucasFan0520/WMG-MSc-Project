// F002.c
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
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *items = NULL;
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
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *msg = p;
            size_t mlen = strlen(msg);
            while (mlen > 0 && (msg[mlen-1] == '\n' || msg[mlen-1] == '\n')) {
                msg[mlen-1] = '\0';
                mlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Record *nitems = realloc(items, cap * sizeof(Record));
                if (nitems) items = nitems;
            }
            items[count].tag = mystrdup(tag);
            items[count].message = mystrdup(msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *newtag = p;
            size_t ntlen = strlen(newtag);
            while (ntlen > 0 && (newtag[ntlen-1] == '\n' || newtag[ntlen-1] == '\n')) {
                newtag[ntlen-1] = '\0';
                ntlen--;
            }
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(items[i].tag, tag1) == 0) m1 = items[i].message;
                if (!m2 && strcmp(items[i].tag, tag2) == 0) m2 = items[i].message;
            }
            if (m1 && m2) {
                size_t l1 = strlen(m1);
                size_t l2 = strlen(m2);
                char *nm = malloc(l1 + l2 + 2);
                if (nm) {
                    strcpy(nm, m1);
                    strcat(nm, " ");
                    strcat(nm, m2);
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Record *nitems = realloc(items, cap * sizeof(Record));
                        if (nitems) items = nitems;
                    }
                    items[count].tag = mystrdup(newtag);
                    items[count].message = nm;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = p;
            size_t tlen = strlen(tag);
            while (tlen > 0 && (tag[tlen-1] == '\n' || tag[tlen-1] == '\n')) {
                tag[tlen-1] = '\0';
                tlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].tag, tag) == 0) {
                    free(items[i].tag);
                    free(items[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = p;
            size_t tlen = strlen(tag);
            while (tlen > 0 && (tag[tlen-1] == '\n' || tag[tlen-1] == '\n')) {
                tag[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].tag, tag) == 0) {
                    char *t = items[i].message;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].tag, items[i].message);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].tag);
        free(items[i].message);
    }
    free(items);
    return 0;
}
