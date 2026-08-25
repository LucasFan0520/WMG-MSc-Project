// F015.c
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

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

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
    free(c->name); free(c->email); free(c->phone); free(c->note);
    c->name = NULL; c->email = NULL; c->phone = NULL; c->note = NULL;
}

static void cmd_add(char *name, char *email, char *phone) {
    if (find_contact(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].email = safe_strdup(email);
    arr[cnt].phone = safe_strdup(phone);
    arr[cnt].note = safe_strdup("");
    if (!arr[cnt].name || !arr[cnt].email || !arr[cnt].phone || !arr[cnt].note) {
        free_contact(&arr[cnt]);
        return;
    }
    cnt++;
}

static void cmd_note(char *name, char *text) {
    int idx = find_contact(name);
    if (idx < 0) return;
    char *nn = safe_strdup(text);
    if (!nn) return;
    free(arr[idx].note);
    arr[idx].note = nn;
}

static void cmd_merge(char *keep, char *remove) {
    int ik = find_contact(keep);
    int ir = find_contact(remove);
    if (ik < 0 || ir < 0 || ik == ir) return;
    size_t lk = strlen(arr[ik].note);
    size_t lr = strlen(arr[ir].note);
    size_t total = lk + (lr > 0 ? 1 : 0) + lr;
    char *merged = malloc(total + 1);
    if (!merged) return;
    memcpy(merged, arr[ik].note, lk);
    if (lr > 0) {
        merged[lk] = ' ';
        memcpy(merged + lk + 1, arr[ir].note, lr);
    }
    merged[total] = '\0';
    free(arr[ik].note);
    arr[ik].note = merged;
    free_contact(&arr[ir]);
    for (size_t i = (size_t)ir; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
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
    printf("%s %s %s %s\n", arr[idx].name, arr[idx].email, arr[idx].phone, arr[idx].note);
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s %s %s\n", arr[i].name, arr[i].email, arr[i].phone, arr[i].note);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_contact(&arr[i]);
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
        if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0}, email[256] = {0}, phone[256] = {0};
            if (sscanf(rest, "%255s %255s %255s", name, email, phone) < 3) continue;
            cmd_add(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *text = rest + n2;
            while (*text == ' ') text++;
            cmd_note(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256] = {0}, remove[256] = {0};
            if (sscanf(rest, "%255s %255s", keep, remove) < 2) continue;
            cmd_merge(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(name);
            else cmd_find(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}