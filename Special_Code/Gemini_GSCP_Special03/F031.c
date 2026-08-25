// F031.c
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
    char *name;
    char *note;
} BatchRecord;

int main(void) {
    BatchRecord *items = NULL;
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
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_s = p;
            size_t nlen = strlen(n_s);
            while (nlen > 0 && (n_s[nlen-1] == '\n' || n_s[nlen-1] == '\n')) {
                n_s[nlen-1] = '\0';
                nlen--;
            }
            int n = atoi(n_s);
            int valid = 1;
            BatchRecord *temp = malloc(n * sizeof(BatchRecord));
            for (int i = 0; i < n; i++) {
                char *bline = read_line();
                if (!bline) {
                    valid = 0;
                    break;
                }
                char *bp = bline;
                while (*bp == ' ') bp++;
                if (*bp == '\0') {
                    valid = 0;
                    free(bline);
                    continue;
                }
                char *rname = bp;
                while (*bp && *bp != ' ') bp++;
                if (*bp) {
                    *bp = '\0';
                    bp++;
                    while (*bp == ' ') bp++;
                }
                char *rnote = bp;
                size_t rnlen = strlen(rnote);
                while (rnlen > 0 && (rnote[rnlen-1] == '\n' || rnote[rnlen-1] == '\n')) {
                    rnote[rnlen-1] = '\0';
                    rnlen--;
                }
                if (strlen(rname) == 0 || strlen(rnote) == 0) {
                    valid = 0;
                }
                if (valid) {
                    temp[i].name = mystrdup(rname);
                    temp[i].note = mystrdup(rnote);
                }
                free(bline);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        BatchRecord *nitems = realloc(items, cap * sizeof(BatchRecord));
                        if (nitems) items = nitems;
                    }
                    items[count] = temp[i];
                    count++;
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (valid) {
                        free(temp[i].name);
                        free(temp[i].note);
                    }
                }
            }
            free(temp);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s\n", items[i].name, items[i].note);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    return 0;
}
