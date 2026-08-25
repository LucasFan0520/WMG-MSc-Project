// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *tag;
    char *msg;
} Record;

int main() {
    Record *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
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
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *msg = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(Record));
            }
            items[count].tag = mystrdup(tag);
            items[count].msg = mystrdup(msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ') p++;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *newtag = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(items[i].tag, tag1) == 0) {
                    m1 = items[i].msg;
                }
                if (!m2 && strcmp(items[i].tag, tag2) == 0) {
                    m2 = items[i].msg;
                }
            }
            if (m1 && m2) {
                size_t l1 = strlen(m1);
                size_t l2 = strlen(m2);
                char *nm = malloc(l1 + l2 + 2);
                strcpy(nm, m1);
                strcat(nm, " ");
                strcat(nm, m2);
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    items = realloc(items, cap * sizeof(Record));
                }
                items[count].tag = mystrdup(newtag);
                items[count].msg = nm;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].tag, tag) == 0) {
                    free(items[i].tag);
                    free(items[i].msg);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
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
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].tag, tag) == 0) {
                    char *t = items[i].msg;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].tag, items[i].msg);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].tag);
        free(items[i].msg);
    }
    free(items);
    return 0;
}
