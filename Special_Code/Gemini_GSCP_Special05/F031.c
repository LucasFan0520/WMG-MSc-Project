// F031.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            long long n = strtoll(line + 6, NULL, 10);
            int batch_ok = 1;
            Record *batch_items = NULL;
            if (n > 0) {
                batch_items = malloc(n * sizeof(Record));
                if (!batch_items) batch_ok = 0;
            }
            for (long long i = 0; i < n; i++) {
                char *bline = read_line(stdin);
                if (!bline) {
                    batch_ok = 0;
                    continue;
                }
                if (!batch_ok) {
                    free(bline);
                    continue;
                }
                char *p = bline;
                while (*p && *p != ' ') p++;
                if (*p == ' ' && p != bline) {
                    *p = '\0';
                    batch_items[i].name = safe_dup(bline);
                    batch_items[i].note = safe_dup(p + 1);
                } else {
                    batch_ok = 0;
                }
                free(bline);
            }
            if (batch_ok && n > 0) {
                while (count + n > cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Record *nb = realloc(db, cap * sizeof(Record));
                    if (!nb) {
                        batch_ok = 0;
                        break;
                    }
                    db = nb;
                }
                if (batch_ok) {
                    for (long long i = 0; i < n; i++) {
                        db[count++] = batch_items[i];
                    }
                } else {
                    for (long long i = 0; i < n; i++) {
                        free(batch_items[i].name);
                        free(batch_items[i].note);
                    }
                }
            } else if (batch_items) {
                for (long long i = 0; i < n; i++) {
                    if (batch_ok || i < count) {
                    }
                }
                free(batch_items);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(db[i].name, name) == 0) {
                    free(db[i].name);
                    free(db[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        db[j] = db[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(db[i].name, name) == 0) {
                    printf("%s\n", db[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
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
