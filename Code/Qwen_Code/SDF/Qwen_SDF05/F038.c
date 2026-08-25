// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 8192
#define MAX_COUNT 1000000

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *name; char *encoded; } Encoded;
static Encoded *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Encoded *t = realloc(arr, nc * sizeof(Encoded));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_encoded(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void cmd_store(char *name, char *encoded) {
    if (find_encoded(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].encoded = safe_strdup(encoded);
    if (!arr[cnt].name || !arr[cnt].encoded) {
        free(arr[cnt].name); free(arr[cnt].encoded);
        arr[cnt].name = NULL; arr[cnt].encoded = NULL;
        return;
    }
    cnt++;
}

static void cmd_decode(char *name) {
    int idx = find_encoded(name);
    if (idx < 0) return;
    char *p = arr[idx].encoded;
    while (*p) {
        if (!isdigit((unsigned char)*p)) return;
        long count = 0;
        while (isdigit((unsigned char)*p)) {
            count = count * 10 + (*p - '0');
            if (count > MAX_COUNT) return;
            p++;
        }
        if (!*p) return;
        char ch = *p++;
        for (long i = 0; i < count; i++)
            putchar(ch == ' ' ? '_' : ch);
    }
    putchar('\n');
}

static void cmd_delete(char *name) {
    int idx = find_encoded(name);
    if (idx < 0) return;
    free(arr[idx].name); free(arr[idx].encoded);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s\n", arr[i].name);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].encoded); }
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
        if (strcmp(cmd, "STORE") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *encoded = rest + n2;
            while (*encoded == ' ') encoded++;
            cmd_store(name, encoded);
        } else if (strcmp(cmd, "DECODE") == 0 || strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            if (strcmp(cmd, "DECODE") == 0) cmd_decode(name);
            else cmd_delete(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}