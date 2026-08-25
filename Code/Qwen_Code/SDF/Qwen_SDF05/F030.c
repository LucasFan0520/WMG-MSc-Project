// F030.c
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

typedef struct { char *old_str; char *new_str; } Rule;
static Rule *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Rule *t = realloc(arr, nc * sizeof(Rule));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_rule(const char *old_str) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].old_str, old_str) == 0) return (int)i;
    return -1;
}

static void cmd_add(char *old_str, char *new_str) {
    if (find_rule(old_str) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].old_str = safe_strdup(old_str);
    arr[cnt].new_str = safe_strdup(new_str);
    if (!arr[cnt].old_str || !arr[cnt].new_str) {
        free(arr[cnt].old_str); free(arr[cnt].new_str);
        arr[cnt].old_str = NULL; arr[cnt].new_str = NULL;
        return;
    }
    cnt++;
}

static void cmd_apply(char *text) {
    for (size_t i = 0; i < cnt; i++) {
        char *pos = strstr(text, arr[i].old_str);
        if (pos) {
            size_t old_len = strlen(arr[i].old_str);
            size_t new_len = strlen(arr[i].new_str);
            size_t pre_len = (size_t)(pos - text);
            size_t post_len = strlen(pos + old_len);
            size_t total = pre_len + new_len + post_len;
            char *res = malloc(total + 1);
            if (!res) return;
            memcpy(res, text, pre_len);
            memcpy(res + pre_len, arr[i].new_str, new_len);
            memcpy(res + pre_len + new_len, pos + old_len, post_len + 1);
            printf("%s\n", res);
            free(res);
            return;
        }
    }
    printf("%s\n", text);
}

static void cmd_delete(char *old_str) {
    int idx = find_rule(old_str);
    if (idx < 0) return;
    free(arr[idx].old_str); free(arr[idx].new_str);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_swap(int i, int j) {
    if (i < 0 || j < 0 || (size_t)i >= cnt || (size_t)j >= cnt || i == j) return;
    Rule tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

static void cmd_rules(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].old_str, arr[i].new_str);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].old_str); free(arr[i].new_str); }
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
            char old_str[256] = {0}, new_str[256] = {0};
            if (sscanf(rest, "%255s %255s", old_str, new_str) < 2) continue;
            cmd_add(old_str, new_str);
        } else if (strcmp(cmd, "APPLY") == 0) {
            cmd_apply(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old_str[256] = {0};
            if (sscanf(rest, "%255s", old_str) < 1) continue;
            cmd_delete(old_str);
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i = 0, j = 0;
            if (sscanf(rest, "%d %d", &i, &j) < 2) continue;
            cmd_swap(i, j);
        } else if (strcmp(cmd, "RULES") == 0) {
            cmd_rules();
        }
    }
    cleanup();
    return 0;
}