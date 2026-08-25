// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

int main(void) {
    Employee *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            char *name = id_end;
            if (*id_end != '\0') {
                *id_end = '\0';
                name++;
            }
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *dept = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                dept++;
            }
            while (*dept == ' ') dept++;
            char *dept_end = dept;
            while (*dept_end && *dept_end != ' ') dept_end++;
            char *title = dept_end;
            if (*dept_end != '\0') {
                *dept_end = '\0';
                title++;
            }
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Employee *new_list = realloc(list, capacity * sizeof(Employee));
                if (new_list) list = new_list;
            }
            list[count].id = mystrdup(id);
            list[count].name = mystrdup(name);
            list[count].department = mystrdup(dept);
            list[count].title = mystrdup(title);
            count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            char *name = id_end;
            if (*id_end != '\0') {
                *id_end = '\0';
                name++;
            }
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *dept = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                dept++;
            }
            while (*dept == ' ') dept++;
            char *dept_end = dept;
            while (*dept_end && *dept_end != ' ') dept_end++;
            char *title = dept_end;
            if (*dept_end != '\0') {
                *dept_end = '\0';
                title++;
            }
            while (*title == ' ') title++;
            char *title_end = title;
            while (*title_end && *title_end != ' ') title_end++;
            *title_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].id, id) == 0) {
                    free(list[i].name);
                    free(list[i].department);
                    free(list[i].title);
                    list[i].name = mystrdup(name);
                    list[i].department = mystrdup(dept);
                    list[i].title = mystrdup(title);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
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
            char *id = p;
            while (*id == ' ') id++;
            char *id_end = id;
            while (*id_end && *id_end != ' ') id_end++;
            *id_end = '\0';
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
