// F028.c
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

typedef struct { char *stop; char *note; } Stop;
static Stop *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Stop *t = realloc(arr, nc * sizeof(Stop));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_stop(const char *stop) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].stop, stop) == 0) return (int)i;
    return -1;
}

static void cmd_add(char *stop, char *note) {
    if (grow() < 0) return;
    arr[cnt].stop = safe_strdup(stop);
    arr[cnt].note = safe_strdup(note);
    if (!arr[cnt].stop || !arr[cnt].note) {
        free(arr[cnt].stop); free(arr[cnt].note);
        arr[cnt].stop = NULL; arr[cnt].note = NULL;
        return;
    }
    cnt++;
}

static void cmd_insert(size_t idx, char *stop, char *note) {
    if (idx > cnt) return;
    if (grow() < 0) return;
    for (size_t i = cnt; i > idx; i--) arr[i] = arr[i - 1];
    arr[idx].stop = safe_strdup(stop);
    arr[idx].note = safe_strdup(note);
    if (!arr[idx].stop || !arr[idx].note) {
        free(arr[idx].stop); free(arr[idx].note);
        for (size_t i = idx; i < cnt; i--) arr[i] = arr[i + 1];
        return;
    }
    cnt++;
}

static void cmd_move(size_t from, size_t to) {
    if (from >= cnt || to >= cnt || from == to) return;
    Stop tmp = arr[from];
    if (from < to) {
        for (size_t i = from; i < to; i++) arr[i] = arr[i + 1];
    } else {
        for (size_t i = from; i > to; i--) arr[i] = arr[i - 1];
    }
    arr[to] = tmp;
}

static void cmd_remove(size_t idx) {
    if (idx >= cnt) return;
    free(arr[idx].stop); free(arr[idx].note);
    for (size_t i = idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_find(char *stop) {
    int idx = find_stop(stop);
    if (idx < 0) return;
    printf("%s\n", arr[idx].note);
}

static void cmd_print(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%s %s\n", arr[i].stop, arr[i].note);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].stop); free(arr[i].note); }
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
            char stop[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", stop, &n2) < 1) continue;
            char *note = rest + n2;
            while (*note == ' ') note++;
            cmd_add(stop, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx_val = 0; char stop[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%d%n %255s%n", &idx_val, &n2, stop, &n3) < 2) continue;
            if (idx_val < 0) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            cmd_insert((size_t)idx_val, stop, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from_val = 0, to_val = 0;
            if (sscanf(rest, "%d %d", &from_val, &to_val) < 2) continue;
            if (from_val < 0 || to_val < 0) continue;
            cmd_move((size_t)from_val, (size_t)to_val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx_val = 0;
            if (sscanf(rest, "%d", &idx_val) < 1) continue;
            if (idx_val < 0) continue;
            cmd_remove((size_t)idx_val);
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256] = {0};
            if (sscanf(rest, "%255s", stop) < 1) continue;
            cmd_find(stop);
        } else if (strcmp(cmd, "PRINT") == 0) {
            cmd_print();
        }
    }
    cleanup();
    return 0;
}