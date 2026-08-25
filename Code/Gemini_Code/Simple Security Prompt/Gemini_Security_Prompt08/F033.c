// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

int main(void) {
    Employee *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *id = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = 1;
                    if (strcmp(cmd, "UPDATE") == 0) {
                        free(list[i].name);
                        free(list[i].dept);
                        free(list[i].title);
                        list[i].name = strdup(name);
                        list[i].dept = strdup(dept);
                        list[i].title = strdup(title);
                    }
                    break;
                }
            }
            if (!found && strcmp(cmd, "ADD") == 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Employee *nb = realloc(list, cap * sizeof(Employee));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].id = strdup(id);
                    list[count].name = strdup(name);
                    list[count].dept = strdup(dept);
                    list[count].title = strdup(title);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, arg1) == 0) {
                    free(list[i].id);
                    free(list[i].name);
                    free(list[i].dept);
                    free(list[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, arg1) == 0) {
                    printf("ID: %s Name: %s Dept: %s Title: %s\n", list[i].id, list[i].name, list[i].dept, list[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("ID: %s Name: %s Dept: %s Title: %s\n", list[i].id, list[i].name, list[i].dept, list[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].name);
        free(list[i].dept);
        free(list[i].title);
    }
    free(list);
    return 0;
}
