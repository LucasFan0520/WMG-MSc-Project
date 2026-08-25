// F003.c
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

typedef struct { char *name; char *exp; } Alias;
static Alias *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Alias *t = realloc(arr, nc * sizeof(Alias));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_alias(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void cmd_define(char *name, char *exp) {
    int idx = find_alias(name);
    if (idx >= 0) {
        char *ne = safe_strdup(exp);
        if (!ne) return;
        free(arr[idx].exp);
        arr[idx].exp = ne;
        return;
    }
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].exp = safe_strdup(exp);
    if (!arr[cnt].name || !arr[cnt].exp) {
        free(arr[cnt].name); free(arr[cnt].exp);
        arr[cnt].name = NULL; arr[cnt].exp = NULL;
        return;
    }
    cnt++;
}

static void cmd_expand(char *name) {
    int idx = find_alias(name);
    if (idx < 0) return;
    for (char *p = arr[idx].exp; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void cmd_rename(char *old, char *new_name) {
    int idx = find_alias(old);
    if (idx < 0) return;
    if (find_alias(new_name) >= 0) return;
    char *nn = safe_strdup(new_name);
    if (!nn) return;
    free(arr[idx].name);
    arr[idx].name = nn;
}

static void cmd_erase(char *name) {
    int idx = find_alias(name);
    if (idx < 0) return;
    free(arr[idx].name); free(arr[idx].exp);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_dump(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].name, arr[i].exp);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].exp); }
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
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *exp = rest + n2;
            while (*exp == ' ') exp++;
            cmd_define(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_expand(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old_n[256] = {0}, new_n[256] = {0};
            if (sscanf(rest, "%255s %255s", old_n, new_n) < 2) continue;
            cmd_rename(old_n, new_n);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_erase(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            cmd_dump();
        }
    }
    cleanup();
    return 0;
}