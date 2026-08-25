// F001.c
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
    char *id;
    char *text;
} Paragraph;

int main(void) {
    Paragraph *items = NULL;
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
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *text = p;
            size_t tlen = strlen(text);
            while (tlen > 0 && (text[tlen-1] == '\n' || text[tlen-1] == '\n')) {
                text[tlen-1] = '\0';
                tlen--;
            }
            if (strcmp(cmd, "NEW") == 0) {
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].id, id) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(items[found].text);
                    items[found].text = mystrdup(text);
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Paragraph *nitems = realloc(items, cap * sizeof(Paragraph));
                        if (nitems) items = nitems;
                    }
                    items[count].id = mystrdup(id);
                    items[count].text = mystrdup(text);
                    count++;
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].id, id) == 0) {
                        size_t l1 = strlen(items[i].text);
                        size_t l2 = strlen(text);
                        char *nt = malloc(l1 + l2 + 1);
                        if (nt) {
                            strcpy(nt, items[i].text);
                            strcat(nt, text);
                            free(items[i].text);
                            items[i].text = nt;
                        }
                        break;
                    }
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].id, id) == 0) {
                        free(items[i].text);
                        items[i].text = mystrdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            size_t ilen = strlen(id);
            while (ilen > 0 && (id[ilen-1] == '\n' || id[ilen-1] == '\n')) {
                id[ilen-1] = '\0';
                ilen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = p;
            size_t ilen = strlen(id);
            while (ilen > 0 && (id[ilen-1] == '\n' || id[ilen-1] == '\n')) {
                id[ilen-1] = '\0';
                ilen--;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *t = items[found].text;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].id);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].text);
    }
    free(items);
    return 0;
}
