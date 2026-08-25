// F034.c
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

typedef struct { char *name; char *note; } Note;
static Note *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Note *t = realloc(arr, nc * sizeof(Note));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_note(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *tmp = NULL;
    size_t tcnt = 0, tcap = 0;
    char line[MAX_LINE];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256] = {0}; int n2 = 0;
        if (sscanf(line, "%255s%n", name, &n2) < 1) continue;
        char *note = line + n2;
        while (*note == ' ') note++;
        if (tcnt >= tcap) {
            size_t nc = tcap ? tcap * 2 : 8;
            if (nc < tcap) { valid = 0; break; }
            Note *t = realloc(tmp, nc * sizeof(Note));
            if (!t) { valid = 0; break; }
            tmp = t; tcap = nc;
        }
        tmp[tcnt].name = safe_strdup(name);
        tmp[tcnt].note = safe_strdup(note);
        if (!tmp[tcnt].name || !tmp[tcnt].note) {
            free(tmp[tcnt].name); free(tmp[tcnt].note);
            valid = 0; break;
        }
        tcnt++;
    }
    fclose(f);
    if (!valid) {
        for (size_t i = 0; i < tcnt; i++) { free(tmp[i].name); free(tmp[i].note); }
        free(tmp);
        return;
    }
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].note); }
    free(arr);
    arr = tmp; cnt = tcnt; cap = tcap;
}

static void cmd_add(char *name, char *note) {
    if (find_note(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].note = safe_strdup(note);
    if (!arr[cnt].name || !arr[cnt].note) {
        free(arr[cnt].name); free(arr[cnt].note);
        arr[cnt].name = NULL; arr[cnt].note = NULL;
        return;
    }
    cnt++;
}

static void cmd_delete(char *name) {
    int idx = find_note(name);
    if (idx < 0) return;
    free(arr[idx].name); free(arr[idx].note);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].name, arr[i].note);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].name); free(arr[i].note); }
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "LOAD") == 0) {
            load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *note = rest + n2;
            while (*note == ' ') note++;
            cmd_add(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_delete(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}