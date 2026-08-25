// F001.c
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

typedef struct { char *id; char *text; } Para;
static Para *arr = NULL;
static size_t cnt = 0, cap = 0;

static int find_para(const char *id) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].id, id) == 0) return (int)i;
    return -1;
}

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Para *t = realloc(arr, nc * sizeof(Para));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static void cmd_new(char *id, char *text) {
    if (find_para(id) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].id = safe_strdup(id);
    arr[cnt].text = safe_strdup(text);
    if (!arr[cnt].id || !arr[cnt].text) {
        free(arr[cnt].id); free(arr[cnt].text);
        arr[cnt].id = NULL; arr[cnt].text = NULL;
        return;
    }
    cnt++;
}

static void cmd_append(char *id, char *text) {
    int idx = find_para(id);
    if (idx < 0) return;
    size_t ol = strlen(arr[idx].text);
    size_t al = strlen(text);
    if (al > (size_t)-1 - ol - 1) return;
    char *nt = realloc(arr[idx].text, ol + al + 1);
    if (!nt) return;
    memcpy(nt + ol, text, al + 1);
    arr[idx].text = nt;
}

static void cmd_replace(char *id, char *text) {
    int idx = find_para(id);
    if (idx < 0) return;
    char *nt = safe_strdup(text);
    if (!nt) return;
    free(arr[idx].text);
    arr[idx].text = nt;
}

static void cmd_delete(char *id) {
    int idx = find_para(id);
    if (idx < 0) return;
    free(arr[idx].id); free(arr[idx].text);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_show(char *id) {
    int idx = find_para(id);
    if (idx < 0) { printf("NOT_FOUND\n"); return; }
    for (char *p = arr[idx].text; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++) printf("%s\n", arr[i].id);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].id); free(arr[i].text); }
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
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char id[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", id, &n2) < 1) continue;
            char *text = rest + n2;
            while (*text == ' ') text++;
            if (strcmp(cmd, "NEW") == 0) cmd_new(id, text);
            else if (strcmp(cmd, "APPEND") == 0) cmd_append(id, text);
            else cmd_replace(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256] = {0};
            if (sscanf(rest, "%255s", id) < 1) continue;
            cmd_delete(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256] = {0};
            if (sscanf(rest, "%255s", id) < 1) continue;
            cmd_show(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}