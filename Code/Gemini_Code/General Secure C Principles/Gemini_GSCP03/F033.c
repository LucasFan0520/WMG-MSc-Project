// F033.c
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
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main() {
    Employee *items = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", items[i].id, items[i].name, items[i].dept, items[i].title);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Employee *nb = realloc(items, cap * sizeof(Employee));
                if (nb) items = nb;
            }
            items[count].id = strdup(id);
            items[count].name = strdup(name);
            items[count].dept = strdup(dept);
            items[count].title = strdup(title);
            count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].name);
                    free(items[i].dept);
                    free(items[i].title);
                    items[i].name = strdup(name);
                    items[i].dept = strdup(dept);
                    items[i].title = strdup(title);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    free(items[i].id);
                    free(items[i].name);
                    free(items[i].dept);
                    free(items[i].title);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].id, id) == 0) {
                    printf("%s %s %s %s\n", items[i].id, items[i].name, items[i].dept, items[i].title);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].id);
        free(items[i].name);
        free(items[i].dept);
        free(items[i].title);
    }
    free(items);
    return 0;
}
