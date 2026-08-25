// F031.c
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

typedef struct { char *name; char *note; } Record;
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

static void cmd_batch(int n) {
    if (n <= 0) return;
    Record *tmp = malloc((size_t)n * sizeof(Record));
    if (!tmp) return;
    int valid = 1;
    for (int i = 0; i < n; i++) {
        char line[MAX_LINE];
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = '\0';
        char name[256] = {0}; int n2 = 0;
        if (sscanf(line, "%255s%n", name, &n2) < 1) { valid = 0; break; }
        char *note = line + n2;
        while (*note == ' ') note++;
        tmp[i].name = safe_strdup(name);
        tmp[i].note = safe_strdup(note);
        if (!tmp[i].name || !tmp[i].note) {
            free(tmp[i].name); free(tmp[i].note);
            tmp[i].name = NULL; tmp[i].note = NULL;
            valid = 0; break;
        }
    }
    if (!valid) {
        for (int i = 0; i < n; i++) { free(tmp[i].name); free(tmp[i].note); }
        free(tmp);
        return;
    }
    for (int i = 0; i < n; i++) {
        if (grow() < 0) break;
        arr[cnt] = tmp[i];
        cnt++;
    }
    free(tmp);
}

static void cmd_delete(char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    free(arr[idx].name); free(arr[idx].note);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_find(char *name) {
    int idx = find_record(name);
    if (idx < 0) return;
    printf("%s %s\n", arr[idx].name, arr[idx].note);
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].name, arr[i].note);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].note); }
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
        if (strcmp(cmd, "BATCH") == 0) {
            int n = 0;
            if (sscanf(rest, "%d", &n) < 1) continue;
            cmd_batch(n);
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