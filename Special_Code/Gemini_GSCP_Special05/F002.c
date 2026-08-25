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
    char *tag;
    char *message;
} MessageRecord;

int main(void) {
    MessageRecord *collector = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *tag = p;
            while (*p && *p != ' ') p++;
            char *message = "";
            if (*p == ' ') {
                *p = '\0';
                message = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                MessageRecord *nb = realloc(collector, cap * sizeof(MessageRecord));
                if (!nb) {
                    free(line);
                    break;
                }
                collector = nb;
            }
            collector[count].tag = safe_dup(tag);
            collector[count].message = safe_dup(message);
            count++;
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            char *newtag = "";
            if (*p == ' ') {
                *p = '\0';
                newtag = p + 1;
            }
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(collector[i].tag, tag1) == 0) {
                    m1 = collector[i].message;
                }
                if (!m2 && strcmp(collector[i].tag, tag2) == 0) {
                    m2 = collector[i].message;
                }
                if (m1 && m2) break;
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
                        cap = cap == 0 ? 4 : cap * 2;
                        MessageRecord *nb = realloc(collector, cap * sizeof(MessageRecord));
                        if (!nb) {
                            free(nm);
                            free(line);
                            break;
                        }
                        collector = nb;
                    }
                    collector[count].tag = safe_dup(newtag);
                    collector[count].message = nm;
                    count++;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(collector[i].tag, tag) == 0) {
                    free(collector[i].tag);
                    free(collector[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        collector[j] = collector[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(collector[i].tag, tag) == 0) {
                    char *t = collector[i].message;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", collector[i].tag, collector[i].message);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(collector[i].tag);
        free(collector[i].message);
    }
    free(collector);
    return 0;
}
