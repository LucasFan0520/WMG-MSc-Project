// F026.c
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
    char *category;
    char *text;
} Note;

int main(void) {
    Note *items = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *txt = p;
            size_t txlen = strlen(txt);
            while (txlen > 0 && (txt[txlen-1] == '\n' || txt[txlen-1] == '\n')) {
                txt[txlen-1] = '\0';
                txlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Note *nitems = realloc(items, cap * sizeof(Note));
                if (nitems) items = nitems;
            }
            items[count].category = mystrdup(cat);
            items[count].text = mystrdup(txt);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = p;
            size_t ctlen = strlen(cat);
            while (ctlen > 0 && (cat[ctlen-1] == '\n' || cat[ctlen-1] == '\n')) {
                cat[ctlen-1] = '\0';
                ctlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].category, cat) == 0) {
                    free(items[i].category);
                    free(items[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = p;
            size_t ctlen = strlen(cat);
            while (ctlen > 0 && (cat[ctlen-1] == '\n' || cat[ctlen-1] == '\n')) {
                cat[ctlen-1] = '\0';
                ctlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].category, cat) != 0) {
                    free(items[i].category);
                    free(items[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = p;
            size_t wdlen = strlen(word);
            while (wdlen > 0 && (word[wdlen-1] == '\n' || word[wdlen-1] == '\n')) {
                word[wdlen-1] = '\0';
                wdlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strstr(items[i].text, word) != NULL) {
                    printf("[%s] %s\n", items[i].category, items[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%s] %s\n", items[i].category, items[i].text);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].category);
        free(items[i].text);
    }
    free(items);
    return 0;
}
