/* F033.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct emp {
    char *id;
    char *name;
    char *dept;
    char *title;
    struct emp *next;
} emp_t;

emp_t *emps = NULL;
emp_t *tail = NULL;

emp_t *find_emp(const char *id) {
    emp_t *e = emps;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    if (find_emp(id)) return;
    emp_t *e = malloc(sizeof(emp_t));
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
    e->next = NULL;
    if (!e->id || !e->name || !e->dept || !e->title) {
        free(e->id); free(e->name); free(e->dept); free(e->title); free(e);
        return;
    }
    if (tail) tail->next = e;
    else emps = e;
    tail = e;
}

void update_emp(const char *id, const char *name, const char *dept, const char *title) {
    emp_t *e = find_emp(id);
    if (!e) return;
    char *new_name = strdup(name);
    char *new_dept = strdup(dept);
    char *new_title = strdup(title);
    if (!new_name || !new_dept || !new_title) {
        free(new_name); free(new_dept); free(new_title);
        return;
    }
    free(e->name);
    free(e->dept);
    free(e->title);
    e->name = new_name;
    e->dept = new_dept;
    e->title = new_title;
}

void delete_emp(const char *id) {
    emp_t *prev = NULL, *e = emps;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else emps = e->next;
            if (e == tail) tail = prev;
            free(e->id);
            free(e->name);
            free(e->dept);
            free(e->title);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void find_emp_print(const char *id) {
    emp_t *e = find_emp(id);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
}

void list_emps(void) {
    emp_t *e = emps;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
        e = e->next;
    }
}

void free_all(void) {
    emp_t *e = emps;
    while (e) {
        emp_t *nxt = e->next;
        free(e->id);
        free(e->name);
        free(e->dept);
        free(e->title);
        free(e);
        e = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256], c[256], d[256];
        if (sscanf(line, "%9s %255s %255s %255s %255s", cmd, a, b, c, d) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s %255s", a, b, c, d) == 4)
                    add_emp(a, b, c, d);
            } else if (strcmp(cmd, "UPDATE") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s %255s", a, b, c, d) == 4)
                    update_emp(a, b, c, d);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_emp(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_emp_print(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_emps();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}