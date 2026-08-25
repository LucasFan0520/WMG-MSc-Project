/* F002.c */
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
    char *tag;
    char *msg;
} Record;

int main(void) {
    Record *records = NULL;
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
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *msg = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Record *nr = realloc(records, cap * sizeof(Record));
                if (nr) records = nr;
            }
            char *ntag = mystrdup(tag);
            char *nmsg = mystrdup(msg);
            if (ntag && nmsg) {
                records[count].tag = ntag;
                records[count].msg = nmsg;
                count++;
            } else {
                free(ntag);
                free(nmsg);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ') p++;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *newtag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(records[i].tag, tag1) == 0) m1 = records[i].msg;
                if (!m2 && strcmp(records[i].tag, tag2) == 0) m2 = records[i].msg;
            }
            if (m1 && m2) {
                size_t nlen = strlen(m1) + strlen(m2) + 2;
                char *nmsg = malloc(nlen);
                if (nmsg) {
                    sprintf(nmsg, "%s %s", m1, m2);
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Record *nr = realloc(records, cap * sizeof(Record));
                        if (nr) records = nr;
                    }
                    char *nt = mystrdup(newtag);
                    if (nt) {
                        records[count].tag = nt;
                        records[count].msg = nmsg;
                        count++;
                    } else {
                        free(nmsg);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag) == 0) {
                    free(records[i].tag);
                    free(records[i].msg);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *t = records[found].msg;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].msg);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].msg);
    }
    free(records);
    return 0;
}
