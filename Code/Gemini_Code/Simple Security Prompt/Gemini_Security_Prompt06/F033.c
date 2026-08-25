// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main() {
    Employee *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", list[i].id, list[i].name, list[i].department, list[i].title);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *id = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *name = rest2;
                    char *rest3 = p3 + 1;
                    char *p4 = strchr(rest3, ' ');
                    if (p4) {
                        *p4 = '\0';
                        char *dept = rest3;
                        char *title = p4 + 1;
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(list[i].id, id) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (strcmp(cmd, "ADD") == 0 && found == -1) {
                            list = realloc(list, (count + 1) * sizeof(Employee));
                            list[count].id = strdup(id);
                            list[count].name = strdup(name);
                            list[count].department = strdup(dept);
                            list[count].title = strdup(title);
                            count++;
                        } else if (strcmp(cmd, "UPDATE") == 0 && found != -1) {
                            free(list[found].name);
                            free(list[found].department);
                            free(list[found].title);
                            list[found].name = strdup(name);
                            list[found].department = strdup(dept);
                            list[found].title = strdup(title);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].id);
                free(list[found].name);
                free(list[found].department);
                free(list[found].title);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Employee));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    printf("%s %s %s %s\n", list[i].id, list[i].name, list[i].department, list[i].title);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].name);
        free(list[i].department);
        free(list[i].title);
    }
    free(list);
    return 0;
}