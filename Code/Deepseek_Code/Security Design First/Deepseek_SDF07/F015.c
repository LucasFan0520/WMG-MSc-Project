// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
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
        free(c->items[i].note);
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
    ct->note = safe_strdup("");
    if (ct->name && ct->email && ct->phone && ct->note)
        c->count++;
    else {
        free(ct->name); free(ct->email); free(ct->phone); free(ct->note);
    }
}

static void cnt_note(Contacts *c, const char *name, const char *text) {
    int idx = cnt_find(c, name);
    if (idx < 0) return;
    char *new = safe_strdup(text);
    if (!new) return;
    free(c->items[idx].note);
    c->items[idx].note = new;
}

static void cnt_merge(Contacts *c, const char *keep, const char *remove) {
    int ki = cnt_find(c, keep);
    int ri = cnt_find(c, remove);
    if (ki < 0 || ri < 0) return;
    Contact *kp = &c->items[ki];
    Contact *rp = &c->items[ri];
    char *new_note = malloc(strlen(kp->note) + strlen(rp->note) + 2);
    if (!new_note) return;
    sprintf(new_note, "%s %s", kp->note, rp->note);
    free(kp->note);
    kp->note = new_note;
    // delete remove contact
    free(rp->name); free(rp->email); free(rp->phone); free(rp->note);
    for (size_t i = ri; i < c->count - 1; i++)
        c->items[i] = c->items[i+1];
    c->count--;
}

static void cnt_delete(Contacts *c, const char *name) {
    int idx = cnt_find(c, name);
    if (idx < 0) return;
    free(c->items[idx].name);
    free(c->items[idx].email);
    free(c->items[idx].phone);
    free(c->items[idx].note);
    for (size_t i = idx; i < c->count - 1; i++)
        c->items[i] = c->items[i+1];
    c->count--;
}

static void cnt_find_print(Contacts *c, const char *name) {
    int idx = cnt_find(c, name);
    if (idx < 0) return;
    Contact *ct = &c->items[idx];
    printf("%s %s %s ", ct->name, ct->email, ct->phone);
    const char *p = ct->note;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
}

static void cnt_list(Contacts *c) {
    for (size_t i = 0; i < c->count; i++) {
        printf("%s\n", c->items[i].name);
    }
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

int main(void) {
    Contacts c;
    cnt_init(&c);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            cnt_add(&c, a1, a2, a3);
        } else if (strcmp(cmd, "NOTE") == 0) {
            cnt_note(&c, a1, a2); // a2=text
        } else if (strcmp(cmd, "MERGE") == 0) {
            cnt_merge(&c, a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            cnt_delete(&c, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            cnt_find_print(&c, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            cnt_list(&c);
        }
    }
    cnt_free(&c);
    return 0;
}