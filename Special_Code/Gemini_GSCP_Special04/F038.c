/* F038.c */
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
    char *encoded;
} RLE;

int main(void) {
    RLE *records = NULL;
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
        if (*p == ' ') { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *encoded = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                RLE *nr = realloc(records, cap * sizeof(RLE));
                if (nr) records = nr;
            }
            char *nn = mystrdup(name);
            char *ne = mystrdup(encoded);
            if (nn && ne) {
                records[count].name = nn;
                records[count].encoded = ne;
                count++;
            } else {
                free(nn); free(ne);
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *enc = records[i].encoded;
                    int valid = 1;
                    while (*enc && valid) {
                        if (*enc >= '0' && *enc <= '9') {
                            long long repeat = 0;
                            while (*enc >= '0' && *enc <= '9') {
                                repeat = repeat * 10 + (*enc - '0');
                                if (repeat > 10000) { valid = 0; break; }
                                enc++;
                            }
                            if (*enc && valid) {
                                char ch = *enc;
                                if (ch == ' ') ch = '_';
                                for (long long j = 0; j < repeat; j++) putchar(ch);
                                enc++;
                            } else {
                                valid = 0;
                            }
                        } else {
                            char ch = *enc;
                            if (ch == ' ') ch = '_';
                            putchar(ch);
                            enc++;
                        }
                    }
                    if (valid) putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            size_t i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].encoded);
    }
    free(records);
    return 0;
}
