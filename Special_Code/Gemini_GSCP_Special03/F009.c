// F009.c
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
    char *group;
    char *desc;
} Event;

int main(void) {
    Event *items = NULL;
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
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *desc = p;
            size_t dlen = strlen(desc);
            while (dlen > 0 && (desc[dlen-1] == '\n' || desc[dlen-1] == '\n')) {
                desc[dlen-1] = '\0';
                dlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Event *nitems = realloc(items, cap * sizeof(Event));
                if (nitems) items = nitems;
            }
            items[count].group = mystrdup(group);
            items[count].desc = mystrdup(desc);
            count++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *ga = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *gb = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *gnew = p;
            size_t gnlen = strlen(gnew);
            while (gnlen > 0 && (gnew[gnlen-1] == '\n' || gnew[gnlen-1] == '\n')) {
                gnew[gnlen-1] = '\0';
                gnlen--;
            }
            size_t current_count = count;
            for (size_t i = 0; i < current_count; i++) {
                if (strcmp(items[i].group, ga) == 0 || strcmp(items[i].group, gb) == 0) {
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Event *nitems = realloc(items, cap * sizeof(Event));
                        if (nitems) items = nitems;
                    }
                    items[count].group = mystrdup(gnew);
                    items[count].desc = mystrdup(items[i].desc);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = p;
            size_t glen = strlen(group);
            while (glen > 0 && (group[glen-1] == '\n' || group[glen-1] == '\n')) {
                group[glen-1] = '\0';
                glen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].group, group) == 0) {
                    free(items[i].group);
                    free(items[i].desc);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = p;
            size_t glen = strlen(group);
            while (glen > 0 && (group[glen-1] == '\n' || group[glen-1] == '\n')) {
                group[glen-1] = '\0';
                glen--;
            }
            size_t cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].group, group) == 0) {
                    cval++;
                }
            }
            printf("%zu\n", cval);
        } else if (strcmp(cmd, "REPORT") == 0) {
            char **printed = NULL;
            size_t pcount = 0;
            for (size_t i = 0; i < count; i++) {
                int already = 0;
                for (size_t j = 0; j < pcount; j++) {
                    if (strcmp(printed[j], items[i].group) == 0) {
                        already = 1;
                        break;
                    }
                }
                if (!already) {
                    printf("%s\n", items[i].group);
                    printed = realloc(printed, (pcount + 1) * sizeof(char*));
                    printed[pcount] = items[i].group;
                    pcount++;
                }
            }
            free(printed);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].group);
        free(items[i].desc);
    }
    free(items);
    return 0;
}
