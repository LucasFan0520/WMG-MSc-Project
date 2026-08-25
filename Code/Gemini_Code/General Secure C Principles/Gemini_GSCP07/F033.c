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

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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

int main(void) {
    Employee *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
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
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Employee *nl = realloc(list, capacity * sizeof(Employee));
                    if (!nl) break;
                    list = nl;
                }
                list[count].id = strdup(id);
                list[count].name = strdup(name);
                list[count].department = strdup(dept);
                list[count].title = strdup(title);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ') p++;
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
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].name);
                    free(list[i].department);
                    free(list[i].title);
                    list[i].name = strdup(name);
                    list[i].department = strdup(dept);
                    list[i].title = strdup(title);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].id);
                    free(list[i].name);
                    free(list[i].department);
                    free(list[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    printf("%s %s %s %s\n", list[i].id, list[i].name, list[i].department, list[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].id, list[i].name, list[i].department, list[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].id);
        free(list[i].name);
        free(list[i].department);
        free(list[i].title);
    }
    free(list);
    return 0;
}
