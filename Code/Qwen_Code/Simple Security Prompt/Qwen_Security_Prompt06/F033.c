// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Emp {
    char *id;
    char *name;
    char *dept;
    char *title;
    struct Emp *next;
} Emp;

Emp *emps = NULL;

Emp *find_emp(const char *id) {
    for (Emp *e = emps; e; e = e->next) {
        if (strcmp(e->id, id) == 0) return e;
    }
    return NULL;
}

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    if (find_emp(id)) return;
    Emp *e = malloc(sizeof(Emp));
    e->id = strdup(id);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
    e->next = emps;
    emps = e;
}

void update_emp(const char *id, const char *name, const char *dept, const char *title) {
    Emp *e = find_emp(id);
    if (!e) return;
    free(e->name);
    free(e->dept);
    free(e->title);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
}

void delete_emp(const char *id) {
    Emp *prev = NULL, *curr = emps;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else emps = curr->next;
            free(curr->id);
            free(curr->name);
            free(curr->dept);
            free(curr->title);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_print_emp(const char *id) {
    Emp *e = find_emp(id);
    if (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
    }
}

void list_emps() {
    for (Emp *e = emps; e; e = e->next) {
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, id[256] = {0}, name[256] = {0}, dept[256] = {0}, title[256] = {0};
        sscanf(line, "%15s %255s %255s %255s %255s", cmd, id, name, dept, title);
        
        if (strcmp(cmd, "ADD") == 0) add_emp(id, name, dept, title);
        else if (strcmp(cmd, "UPDATE") == 0) update_emp(id, name, dept, title);
        else if (strcmp(cmd, "DELETE") == 0) delete_emp(id);
        else if (strcmp(cmd, "FIND") == 0) find_print_emp(id);
        else if (strcmp(cmd, "LIST") == 0) list_emps();
        
        free(line);
    }
    return 0;
}