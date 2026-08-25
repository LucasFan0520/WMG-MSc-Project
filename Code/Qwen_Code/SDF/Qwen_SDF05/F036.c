// F036.c
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

typedef struct { char *name; char *data; } Record;
static Record *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Record *t = realloc(arr, nc * sizeof(Record));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_record(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void cmd_add(char *name, char *payload) {
    char *colon = strchr(payload, ':');
    if (!colon) return;
    *colon = '\0';
    long len = strtol(payload, NULL, 10);
    if (len < 0) return;
    char *data = colon + 1;
    if ((size_t)len != strlen(data)) return;
    if (find_record(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].data = safe_strdup(data);
    if (!arr[cnt].name || !arr[cnt].data) {
        free(arr[cnt].name); free(arr[cnt].data);
        arr[cnt].name = NULL; arr[cnt].data = NULL;
        return;
    }
    cnt++;
}

static void cmd_delete(char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    free(arr[idx].name); free(arr[idx].data);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_show(char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    for (char *p = arr[idx].data; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s\n", arr[i].name);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].data); }
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
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *payload = rest + n2;
            while (*payload == ' ') payload++;
            cmd_add(name, payload);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "SHOW") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            if (strcmp(cmd, "DELETE") == 0) cmd_delete(name);
            else cmd_show(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}