/* F031.c */
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
} Record;

int main(void) {
    Record *db = NULL;
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
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ') p++;
            char *n_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long n = strtoll(n_str, NULL, 10);
            int valid = 1;
            Record *batch_items = NULL;
            if (n > 0) {
                batch_items = malloc(n * sizeof(Record));
                if (!batch_items) valid = 0;
                for (long long i = 0; i < n; i++) {
                    char *bline = read_line(stdin);
                    if (!bline) {
                        valid = 0;
                        continue;
                    }
                    if (valid) {
                        char *bp = bline;
                        while (*bp == ' ') bp++;
                        char *name = bp;
                        while (*bp && *bp != ' ') bp++;
                        if (*bp == ' ') {
                            *bp = '\0';
                            bp++;
                        }
                        while (*bp == ' ') bp++;
                        char *note = bp;
                        if (strlen(name) == 0) {
                            valid = 0;
                        } else {
                            batch_items[i].name = mystrdup(name);
                            batch_items[i].note = mystrdup(note);
                        }
                    }
                    free(bline);
                }
            }
            if (valid && n > 0) {
                for (long long i = 0; i < n; i++) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Record *nd = realloc(db, cap * sizeof(Record));
                        if (nd) db = nd;
                    }
                    db[count++] = batch_items[i];
                }
                free(batch_items);
            } else {
                if (batch_items) {
                    for (long long i = 0; i < n; i++) {
                        if (valid) { 
                            free(batch_items[i].name);
                            free(batch_items[i].note);
                        }
                    }
                    free(batch_items);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(db[i].name, name) == 0) {
                    free(db[i].name);
                    free(db[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        db[j] = db[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(db[i].name, name) == 0) {
                    printf("%s\n", db[i].note);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", db[i].name, db[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(db[i].name);
        free(db[i].note);
    }
    free(db);
    return 0;
}
