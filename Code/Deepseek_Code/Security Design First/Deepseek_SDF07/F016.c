// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

typedef struct {
    Contact *items;
    size_t count;
    size_t capacity;
} Contacts;

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

static void cnt_init(Contacts *c) {
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static void cnt_free(Contacts *c) {
    for (size_t i = 0; i < c->count; i++) {
        free(c->items[i].name);
        free(c->items[i].email);
        free(c->items[i].phone);
    }
    free(c->items);
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static int cnt_find(Contacts *c, const char *name) {
    for (size_t i = 0; i < c->count; i++) {
        if (strcmp(c->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void cnt_add(Contacts *c, const char *name, const char *email, const char *phone) {
    if (cnt_find(c, name) >= 0) return;
    if (c->count == c->capacity) {
        size_t newcap = c->capacity ? c->capacity * 2 : 4;
        Contact *tmp = realloc(c->items, newcap * sizeof(Contact));
        if (!tmp) return;
        c->items = tmp;
        c->capacity = newcap;
    }
    Contact *ct = &c->items[c->count];
    ct->name = safe_strdup(name);
    ct->email = safe_strdup(email);
    ct->phone = safe_strdup(phone);
    if (ct->name && ct->email && ct->phone)
        c->count++;
    else {
        free(ct->name); free(ct->email); free(ct->phone);
    }
}

static void cnt_delete(Contacts *c, const char *name) {
    int idx = cnt_find(c, name);
    if (idx < 0) return;
    free(c->items[idx].name);
    free(c->items[idx].email);
    free(c->items[idx].phone);
    for (size_t i = idx; i < c->count - 1; i++)
        c->items[i] = c->items[i+1];
    c->count--;
}

static void cnt_find_print(Contacts *c, const char *name) {
    int idx = cnt_find(c, name);
    if (idx < 0) return;
    printf("%s %s %s\n", c->items[idx].name, c->items[idx].email, c->items[idx].phone);
}

static void cnt_report(Contacts *c) {
    for (size_t i = 0; i < c->count; i++) {
        printf("%s %s %s\n", c->items[i].name, c->items[i].email, c->items[i].phone);
    }
}

static void load_file(Contacts *c, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char *name = strtok(line, ",");
        if (!name) continue;
        char *email = strtok(NULL, ",");
        if (!email) continue;
        char *phone = strtok(NULL, ",");
        if (!phone) continue;
        cnt_add(c, name, email, phone);
    }
    fclose(f);
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
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
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(int argc, char **argv) {
    Contacts c;
    cnt_init(&c);
    if (argc > 1) {
        load_file(&c, argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            cnt_add(&c, a1, a2, a3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            cnt_delete(&c, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            cnt_find_print(&c, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cnt_report(&c);
        }
    }
    cnt_free(&c);
    return 0;
}