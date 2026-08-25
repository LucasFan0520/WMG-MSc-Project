// F027.c
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

typedef struct { long priority; char *title; char *note; } Reminder;
static Reminder *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Reminder *t = realloc(arr, nc * sizeof(Reminder));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_title(const char *title) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].title, title) == 0) return (int)i;
    return -1;
}

static void cmd_add(long priority, char *title, char *note) {
    if (find_title(title) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].priority = priority;
    arr[cnt].title = safe_strdup(title);
    arr[cnt].note = safe_strdup(note);
    if (!arr[cnt].title || !arr[cnt].note) {
        free(arr[cnt].title); free(arr[cnt].note);
        arr[cnt].title = NULL; arr[cnt].note = NULL;
        return;
    }
    cnt++;
}

static void cmd_bump(char *title, long delta) {
    int idx = find_title(title);
    if (idx < 0) return;
    arr[idx].priority += delta;
}

static void cmd_remove(char *title) {
    int idx = find_title(title);
    if (idx < 0) return;
    free(arr[idx].title); free(arr[idx].note);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static int cmp_rem(const void *a, const void *b) {
    const Reminder *x = (const Reminder *)a;
    const Reminder *y = (const Reminder *)b;
    if (x->priority != y->priority) return (x->priority < y->priority) - (x->priority > y->priority);
    return 0;
}

static void cmd_top(long n) {
    if (cnt == 0 || n <= 0) return;
    Reminder *sorted = malloc(cnt * sizeof(Reminder));
    if (!sorted) return;
    memcpy(sorted, arr, cnt * sizeof(Reminder));
    qsort(sorted, cnt, sizeof(Reminder), cmp_rem);
    size_t limit = (size_t)n < cnt ? (size_t)n : cnt;
    for (size_t i = 0; i < limit; i++)
        printf("%ld %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
    free(sorted);
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++)
        printf("%ld %s %s\n", arr[i].priority, arr[i].title, arr[i].note);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].title); free(arr[i].note); }
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
            long priority = 0; char title[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%ld%n %255s%n", &priority, &n2, title, &n3) < 2) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            cmd_add(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256] = {0}; long delta = 0;
            if (sscanf(rest, "%255s %ld", title, &delta) < 2) continue;
            cmd_bump(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256] = {0};
            if (sscanf(rest, "%255s", title) < 1) continue;
            cmd_remove(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            long n = 0;
            if (sscanf(rest, "%ld", &n) < 1) continue;
            cmd_top(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}