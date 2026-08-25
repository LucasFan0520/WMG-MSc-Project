// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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

int main(void) {
    Employee *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
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
                        int found = 0;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(arr[i].id, id) == 0) {
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            if (count >= cap) {
                                size_t ncap = cap == 0 ? 4 : cap * 2;
                                Employee *narr = realloc(arr, ncap * sizeof(Employee));
                                if (narr) {
                                    arr = narr;
                                    cap = ncap;
                                }
                            }
                            if (count < cap) {
                                arr[count].id = strdup(id);
                                arr[count].name = strdup(name);
                                arr[count].dept = strdup(dept);
                                arr[count].title = strdup(title);
                                count++;
                            }
                        }
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
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(arr[i].id, id) == 0) {
                                free(arr[i].name);
                                free(arr[i].dept);
                                free(arr[i].title);
                                arr[i].name = strdup(name);
                                arr[i].dept = strdup(dept);
                                arr[i].title = strdup(title);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].name);
                    free(arr[i].dept);
                    free(arr[i].title);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].name);
        free(arr[i].dept);
        free(arr[i].title);
    }
    free(arr);
    return 0;
}
