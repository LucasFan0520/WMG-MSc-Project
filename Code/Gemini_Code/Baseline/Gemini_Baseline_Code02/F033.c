// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Emp;

int main() {
    Emp *emps = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = args;
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = p2 + 1;
                char *p3 = strchr(name, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *dept = p3 + 1;
                    char *p4 = strchr(dept, ' ');
                    if (p4) {
                        *p4 = '\0';
                        char *title = p4 + 1;
                        emps = realloc(emps, sizeof(Emp) * (count + 1));
                        emps[count].id = strdup(id);
                        emps[count].name = strdup(name);
                        emps[count].dept = strdup(dept);
                        emps[count].title = strdup(title);
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = args;
            char *p2 = strchr(id, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = p2 + 1;
                char *p3 = strchr(name, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *dept = p3 + 1;
                    char *p4 = strchr(dept, ' ');
                    if (p4) {
                        *p4 = '\0';
                        char *title = p4 + 1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(emps[i].id, id) == 0) {
                                free(emps[i].name); emps[i].name = strdup(name);
                                free(emps[i].dept); emps[i].dept = strdup(dept);
                                free(emps[i].title); emps[i].title = strdup(title);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(emps[i].id, args) == 0) {
                    free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
                    for (int j = i; j < count - 1; j++) {
                        emps[j] = emps[j + 1];
                    }
                    count--;
                    if (count == 0) { free(emps); emps = NULL; }
                    else { emps = realloc(emps, sizeof(Emp) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(emps[i].id, args) == 0) {
                    printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
    }
    free(emps);
    return 0;
}
