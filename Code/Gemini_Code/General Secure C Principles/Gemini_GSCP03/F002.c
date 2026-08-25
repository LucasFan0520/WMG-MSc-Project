// F002.c
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
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
        if (*p == '\0') {
            free(line);
            continue;
        }
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
        if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].tag, items[i].msg);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *msg = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Record *nb = realloc(items, cap * sizeof(Record));
                if (nb) items = nb;
            }
            items[count].tag = strdup(tag);
            items[count].msg = strdup(msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
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
                size_t nlen = strlen(m1) + 1 + strlen(m2) + 1;
                char *nmsg = malloc(nlen);
                if (nmsg) {
                    strcpy(nmsg, m1);
                    strcat(nmsg, " ");
                    strcat(nmsg, m2);
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        Record *nb = realloc(items, cap * sizeof(Record));
                        if (nb) items = nb;
                    }
                    items[count].tag = strdup(newtag);
                    items[count].msg = nmsg;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; ) {
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
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].tag, tag) == 0) {
                    char *mptr = items[i].msg;
                    while (*mptr) {
                        if (*mptr == ' ') {
                            putchar('_');
                        } else {
                            putchar(*mptr);
                        }
                        mptr++;
                    }
                    putchar('\n');
                    break;
                }
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
