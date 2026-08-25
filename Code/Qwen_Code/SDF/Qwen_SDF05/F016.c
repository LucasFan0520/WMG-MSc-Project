// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *name; char *email; char *phone; } Contact;
static Contact *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Contact *t = realloc(arr, nc * sizeof(Contact));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_contact(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_contact(Contact *c) {
    free(c->name); free(c->email); free(c->phone);
    c->name = NULL; c->email = NULL; c->phone = NULL;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *c1 = strchr(line, ',');
        if (!c1) continue;
        char *c2 = strchr(c1 + 1, ',');
        if (!c2) continue;
        *c1 = '\0'; *c2 = '\0';
        char *name = line;
        char *email = c1 + 1;
        char *phone = c2 + 1;
        if (!*name || !*email || !*phone) continue;
        if (find_contact(name) >= 0) continue;
        if (grow() < 0) break;
        arr[cnt].name = safe_strdup(name);
        arr[cnt].email = safe_strdup(email);
        arr[cnt].phone = safe_strdup(phone);
        if (!arr[cnt].name || !arr[cnt].email || !arr[cnt].phone) {
            free_contact(&arr[cnt]);
            break;
        }
        cnt++;
    }
    fclose(f);
}

static void cmd_add(char *line_rest) {
    char *c1 = strchr(line_rest, ',');
    if (!c1) return;
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return;
    *c1 = '\0'; *c2 = '\0';
    char *name = line_rest;
    char *email = c1 + 1;
    char *phone = c2 + 1;
    if (!*name || !*email || !*phone) return;
    if (find_contact(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].email = safe_strdup(email);
    arr[cnt].phone = safe_strdup(phone);
    if (!arr[cnt].name || !arr[cnt].email || !arr[cnt].phone) {
        free_contact(&arr[cnt]);
        return;
    }
    cnt++;
}

static void cmd_delete(char *name) {
    int idx = find_contact(name);
    if (idx < 0) return;
    free_contact(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_find(char *name) {
    int idx = find_contact(name);
    if (idx < 0) return;
    printf("%s,%s,%s\n", arr[idx].name, arr[idx].email, arr[idx].phone);
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_contact(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            cmd_add(rest);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(name);
            else cmd_find(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}