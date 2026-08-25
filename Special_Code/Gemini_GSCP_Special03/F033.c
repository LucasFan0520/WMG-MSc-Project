// F033.c
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
    char *name;
    char *dept;
    char *title;
} Employee;

int main(void) {
    Employee *items = NULL;
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
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *title = p;
            size_t ttlen = strlen(title);
            while (ttlen > 0 && (title[ttlen-1] == '\n' || title[ttlen-1] == '\n')) {
                title[ttlen-1] = '\0';
                ttlen--;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                if (strcmp(cmd, "UPDATE") == 0 || strcmp(cmd, "ADD") == 0) {
                    free(items[found].name);
                    free(items[found].dept);
                    free(items[found].title);
                    items[found].name = mystrdup(name);
                    items[found].dept = mystrdup(dept);
                    items[found].title = mystrdup(title);
                }
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Employee *nitems = realloc(items, cap * sizeof(Employee));
                    if (nitems) items = nitems;
                }
                items[count].id = mystrdup(id);
                items[count].name = mystrdup(name);
                items[count].dept = mystrdup(dept);
                items[count].title = mystrdup(title);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            size_t idlen = strlen(id);
            while (idlen > 0 && (id[idlen-1] == '\n' || id[idlen-1] == '\n')) {
                id[idlen-1] = '\0';
                idlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].name);
                    free(items[i].dept);
                    free(items[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = p;
            size_t idlen = strlen(id);
            while (idlen > 0 && (id[idlen-1] == '\n' || id[idlen-1] == '\n')) {
                id[idlen-1] = '\0';
                idlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s %s %s %s\n", items[i].id, items[i].name, items[i].dept, items[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", items[i].id, items[i].name, items[i].dept, items[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].name);
        free(items[i].dept);
        free(items[i].title);
    }
    free(items);
    return 0;
}
