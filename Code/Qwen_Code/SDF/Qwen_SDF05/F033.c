// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *id; char *name; char *dept; char *title; } Employee;
static Employee *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Employee *t = realloc(arr, nc * sizeof(Employee));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_emp(const char *id) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].id, id) == 0) return (int)i;
    return -1;
}

static void free_emp(Employee *e) {
    free(e->id); free(e->name); free(e->dept); free(e->title);
    e->id = NULL; e->name = NULL; e->dept = NULL; e->title = NULL;
}

static void cmd_add(char *id, char *name, char *dept, char *title) {
    if (find_emp(id) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].id = safe_strdup(id);
    arr[cnt].name = safe_strdup(name);
    arr[cnt].dept = safe_strdup(dept);
    arr[cnt].title = safe_strdup(title);
    if (!arr[cnt].id || !arr[cnt].name || !arr[cnt].dept || !arr[cnt].title) {
        free_emp(&arr[cnt]);
        return;
    }
    cnt++;
}

static void cmd_update(char *id, char *name, char *dept, char *title) {
    int idx = find_emp(id);
    if (idx < 0) return;
    char *n = safe_strdup(name);
    char *d = safe_strdup(dept);
    char *t = safe_strdup(title);
    if (!n || !d || !t) { free(n); free(d); free(t); return; }
    free(arr[idx].name); free(arr[idx].dept); free(arr[idx].title);
    arr[idx].name = n; arr[idx].dept = d; arr[idx].title = t;
}

static void cmd_delete(char *id) {
    int idx = find_emp(id);
    if (idx < 0) return;
    free_emp(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_find(char *id) {
    int idx = find_emp(id);
    if (idx < 0) return;
    printf("%s %s %s %s\n", arr[idx].id, arr[idx].name, arr[idx].dept, arr[idx].title);
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_emp(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char id[256] = {0}, name[256] = {0}, dept[256] = {0}, title[256] = {0};
            if (sscanf(rest, "%255s %255s %255s %255s", id, name, dept, title) < 4) continue;
            if (strcmp(cmd, "ADD") == 0) cmd_add(id, name, dept, title);
            else cmd_update(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "FIND") == 0) {
            char id[256] = {0};
            if (sscanf(rest, "%255s", id) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(id);
            else cmd_find(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}