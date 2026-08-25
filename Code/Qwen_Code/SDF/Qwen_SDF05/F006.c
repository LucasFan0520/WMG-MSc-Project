// F006.c
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

typedef struct { char *item; long count; } Inv;
static Inv *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Inv *t = realloc(arr, nc * sizeof(Inv));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_item(const char *item) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].item, item) == 0) return (int)i;
    return -1;
}

static void cmd_add(char *item, long amount) {
    int idx = find_item(item);
    if (idx >= 0) {
        arr[idx].count += amount;
        if (arr[idx].count < 0) arr[idx].count = 0;
        return;
    }
    if (grow() < 0) return;
    arr[cnt].item = safe_strdup(item);
    if (!arr[cnt].item) return;
    arr[cnt].count = amount > 0 ? amount : 0;
    cnt++;
}

static void cmd_take(char *item, long amount) {
    int idx = find_item(item);
    if (idx < 0) return;
    arr[idx].count -= amount;
    if (arr[idx].count < 0) arr[idx].count = 0;
}

static void cmd_remove(char *item) {
    int idx = find_item(item);
    if (idx < 0) return;
    free(arr[idx].item);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_count(char *item) {
    int idx = find_item(item);
    if (idx < 0) { printf("0\n"); return; }
    printf("%ld\n", arr[idx].count);
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %ld\n", arr[i].item, arr[i].count);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free(arr[i].item);
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
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "TAKE") == 0) {
            char item[256] = {0}; long amount = 0;
            if (sscanf(rest, "%255s %ld", item, &amount) < 2) continue;
            if (strcmp(cmd, "ADD") == 0) cmd_add(item, amount);
            else cmd_take(item, amount);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char item[256] = {0};
            if (sscanf(rest, "%255s", item) < 1) continue;
            cmd_remove(item);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char item[256] = {0};
            if (sscanf(rest, "%255s", item) < 1) continue;
            cmd_count(item);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}