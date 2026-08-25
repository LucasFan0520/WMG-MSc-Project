// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

typedef struct {
    Employee *items;
    size_t count;
    size_t capacity;
} Registry;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void emp_free(Employee *e) {
    free(e->id);
    free(e->name);
    free(e->department);
    free(e->title);
}

static void reg_init(Registry *r) {
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

static void reg_free(Registry *r) {
    for (size_t i = 0; i < r->count; i++)
        emp_free(&r->items[i]);
    free(r->items);
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

static int reg_find(Registry *r, const char *id) {
    for (size_t i = 0; i < r->count; i++) {
        if (strcmp(r->items[i].id, id) == 0)
            return (int)i;
    }
    return -1;
}

static void reg_add(Registry *r, const char *id, const char *name, const char *dept, const char *title) {
    if (reg_find(r, id) >= 0) return;
    if (r->count == r->capacity) {
        size_t newcap = r->capacity ? r->capacity * 2 : 4;
        Employee *tmp = realloc(r->items, newcap * sizeof(Employee));
        if (!tmp) return;
        r->items = tmp;
        r->capacity = newcap;
    }
    Employee *e = &r->items[r->count];
    e->id = safe_strdup(id);
    e->name = safe_strdup(name);
    e->department = safe_strdup(dept);
    e->title = safe_strdup(title);
    if (e->id && e->name && e->department && e->title)
        r->count++;
    else {
        free(e->id); free(e->name); free(e->department); free(e->title);
    }
}

static void reg_update(Registry *r, const char *id, const char *name, const char *dept, const char *title) {
    int idx = reg_find(r, id);
    if (idx < 0) return;
    Employee *e = &r->items[idx];
    char *new_name = safe_strdup(name);
    char *new_dept = safe_strdup(dept);
    char *new_title = safe_strdup(title);
    if (!new_name || !new_dept || !new_title) {
        free(new_name); free(new_dept); free(new_title);
        return;
    }
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = new_name;
    e->department = new_dept;
    e->title = new_title;
}

static void reg_delete(Registry *r, const char *id) {
    int idx = reg_find(r, id);
    if (idx < 0) return;
    emp_free(&r->items[idx]);
    for (size_t i = idx; i < r->count - 1; i++)
        r->items[i] = r->items[i+1];
    r->count--;
}

static void reg_find_print(Registry *r, const char *id) {
    int idx = reg_find(r, id);
    if (idx < 0) return;
    printf("%s %s %s\n", r->items[idx].name, r->items[idx].department, r->items[idx].title);
}

static void reg_list(Registry *r) {
    for (size_t i = 0; i < r->count; i++) {
        printf("%s\n", r->items[i].id);
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz, char *a4, size_t a4sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a4sz - 1) a4[i++] = *p++;
    a4[i] = '\0';
}

int main(void) {
    Registry r;
    reg_init(&r);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256], a4[256];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3), a4, sizeof(a4));
        if (strcmp(cmd, "ADD") == 0) {
            reg_add(&r, a1, a2, a3, a4);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            reg_update(&r, a1, a2, a3, a4);
        } else if (strcmp(cmd, "DELETE") == 0) {
            reg_delete(&r, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            reg_find_print(&r, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            reg_list(&r);
        }
    }
    reg_free(&r);
    return 0;
}