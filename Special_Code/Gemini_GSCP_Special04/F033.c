/* F033.c */
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
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main(void) {
    Employee *registry = NULL;
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
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Employee *nr = realloc(registry, cap * sizeof(Employee));
                    if (nr) registry = nr;
                }
                char *nid = mystrdup(id);
                char *nname = mystrdup(name);
                char *ndept = mystrdup(dept);
                char *ntitle = mystrdup(title);
                if (nid && nname && ndept && ntitle) {
                    registry[count].id = nid;
                    registry[count].name = nname;
                    registry[count].dept = ndept;
                    registry[count].title = ntitle;
                    count++;
                } else {
                    free(nid); free(nname); free(ndept); free(ntitle);
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nname = mystrdup(name);
                char *ndept = mystrdup(dept);
                char *ntitle = mystrdup(title);
                if (nname && ndept && ntitle) {
                    free(registry[found].name);
                    free(registry[found].dept);
                    free(registry[found].title);
                    registry[found].name = nname;
                    registry[found].dept = ndept;
                    registry[found].title = ntitle;
                } else {
                    free(nname); free(ndept); free(ntitle);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(registry[found].id);
                free(registry[found].name);
                free(registry[found].dept);
                free(registry[found].title);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    registry[i] = registry[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(registry[i].id, id) == 0) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].dept);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
