// F033.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

Employee *registry = NULL;
int count = 0;
int capacity = 0;

int find_employee(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(registry[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id) != -1) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        registry = realloc(registry, capacity * sizeof(Employee));
    }
    registry[count].id = strdup(id);
    registry[count].name = strdup(name);
    registry[count].department = strdup(dept);
    registry[count].title = strdup(title);
    count++;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    int idx = find_employee(id);
    if (idx != -1) {
        free(registry[idx].name);
        free(registry[idx].department);
        free(registry[idx].title);
        registry[idx].name = strdup(name);
        registry[idx].department = strdup(dept);
        registry[idx].title = strdup(title);
    }
}

void delete_employee(const char *id) {
    int idx = find_employee(id);
    if (idx != -1) {
        free(registry[idx].id);
        free(registry[idx].name);
        free(registry[idx].department);
        free(registry[idx].title);
        for (int i = idx; i < count - 1; i++) {
            registry[i] = registry[i + 1];
        }
        count--;
    }
}

void find_employee_p(const char *id) {
    int idx = find_employee(id);
    if (idx != -1) {
        printf("%s %s %s %s\n", registry[idx].id, registry[idx].name, registry[idx].department, registry[idx].title);
    }
}

void list_employees(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s %s %s\n", registry[i].id, registry[i].name, registry[i].department, registry[i].title);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *dept = strtok(NULL, " ");
            char *title = strtok(NULL, "");
            if (id && name && dept && title) add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *dept = strtok(NULL, " ");
            char *title = strtok(NULL, "");
            if (id && name && dept && title) update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(NULL, "");
            if (id) delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = strtok(NULL, "");
            if (id) find_employee_p(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(registry[i].id);
        free(registry[i].name);
        free(registry[i].department);
        free(registry[i].title);
    }
    free(registry);
    return 0;
}
