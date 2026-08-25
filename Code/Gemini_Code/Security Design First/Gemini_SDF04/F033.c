// F033.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

Employee *registry = NULL;
int count = 0;
int capacity = 0;

int find_emp(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(registry[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *name = space2 + 1;
                    char *space3 = strchr(name, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *dept = space3 + 1;
                        char *space4 = strchr(dept, ' ');
                        if (space4) {
                            *space4 = '\0';
                            char *title = space4 + 1;
                            if (find_emp(arg1) == -1) {
                                if (count >= capacity) {
                                    capacity = capacity == 0 ? 4 : capacity * 2;
                                    Employee *tmp = realloc(registry, capacity * sizeof(Employee));
                                    if (!tmp) {
                                        free(line);
                                        return 1;
                                    }
                                    registry = tmp;
                                }
                                registry[count].id = strdup(arg1);
                                registry[count].name = strdup(name);
                                registry[count].dept = strdup(dept);
                                registry[count].title = strdup(title);
                                if (registry[count].id && registry[count].name && registry[count].dept && registry[count].title) {
                                    count++;
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "UPDATE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *name = space2 + 1;
                    char *space3 = strchr(name, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *dept = space3 + 1;
                        char *space4 = strchr(dept, ' ');
                        if (space4) {
                            *space4 = '\0';
                            char *title = space4 + 1;
                            int idx = find_emp(arg1);
                            if (idx != -1) {
                                char *nn = strdup(name);
                                char *nd = strdup(dept);
                                char *nt = strdup(title);
                                if (nn && nd && nt) {
                                    free(registry[idx].name);
                                    free(registry[idx].dept);
                                    free(registry[idx].title);
                                    registry[idx].name = nn;
                                    registry[idx].dept = nd;
                                    registry[idx].title = nt;
                                } else {
                                    free(nn); free(nd); free(nt);
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_emp(arg1);
                if (idx != -1) {
                    free(registry[idx].id);
                    free(registry[idx].name);
                    free(registry[idx].dept);
                    free(registry[idx].title);
                    for (int i = idx; i < count - 1; i++) {
                        registry[i] = registry[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int idx = find_emp(arg1);
                if (idx != -1) {
                    printf("%s %s %s %s\n", registry[idx].id, registry[idx].name, registry[idx].dept, registry[idx].title);
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].dept, registry[i].title);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].dept);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}