// F013.c
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
    char *title;
    char *note;
    char *list;
} Task;

int main(void) {
    Task *items = NULL;
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
        if (strcmp(cmd, "TODO") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Task *nitems = realloc(items, cap * sizeof(Task));
                if (nitems) items = nitems;
            }
            items[count].title = mystrdup(title);
            items[count].note = mystrdup(note);
            items[count].list = mystrdup("TODO");
            count++;
        } else if (strcmp(cmd, "START") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0 && strcmp(items[i].list, "TODO") == 0) {
                    free(items[i].list);
                    items[i].list = mystrdup("DOING");
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0 && strcmp(items[i].list, "DOING") == 0) {
                    free(items[i].list);
                    items[i].list = mystrdup("DONE");
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].note);
                    free(items[i].list);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            size_t tlen = strlen(title);
            while (tlen > 0 && (title[tlen-1] == '\n' || title[tlen-1] == '\n')) {
                title[tlen-1] = '\0';
                tlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    printf("%s %s\n", items[i].list, items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].list, "TODO") == 0) {
                    printf("  %s %s\n", items[i].title, items[i].note);
                }
            }
            printf("DOING:\n");
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].list, "DOING") == 0) {
                    printf("  %s %s\n", items[i].title, items[i].note);
                }
            }
            printf("DONE:\n");
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].list, "DONE") == 0) {
                    printf("  %s %s\n", items[i].title, items[i].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
        free(items[i].list);
    }
    free(items);
    return 0;
}
