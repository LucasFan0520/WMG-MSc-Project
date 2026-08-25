/* F029.c */
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
    char *key;
    char *val;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *val = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Record *nd = realloc(db, cap * sizeof(Record));
                if (nd) db = nd;
            }
            char *nk = mystrdup(key);
            char *nv = mystrdup(val);
            if (nk && nv) {
                db[count].key = nk;
                db[count].val = nv;
                count++;
            } else {
                free(nk); free(nv);
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            size_t i = 0;
            while (i < count) {
                int first = 1;
                for (size_t j = 0; j < i; j++) {
                    if (strcmp(db[j].key, db[i].key) == 0) {
                        first = 0;
                        break;
                    }
                }
                if (!first) {
                    free(db[i].key);
                    free(db[i].val);
                    for (size_t j = i; j < count - 1; j++) {
                        db[j] = db[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(db[i].key, key) == 0) {
                    free(db[i].key);
                    free(db[i].val);
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
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(db[i].key, key) == 0) {
                    printf("%s\n", db[i].val);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", db[i].key, db[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(db[i].key);
        free(db[i].val);
    }
    free(db);
    return 0;
}
