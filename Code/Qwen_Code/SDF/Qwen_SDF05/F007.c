// F007.c
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

typedef struct { long time; char *label; char *note; } Activity;
static Activity *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Activity *t = realloc(arr, nc * sizeof(Activity));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int cmp_act(const void *a, const void *b) {
    const Activity *x = (const Activity *)a;
    const Activity *y = (const Activity *)b;
    if (x->time != y->time) return (x->time > y->time) - (x->time < y->time);
    return strcmp(x->label, y->label);
}

static void cmd_add(long time, char *label, char *note) {
    if (grow() < 0) return;
    arr[cnt].time = time;
    arr[cnt].label = safe_strdup(label);
    arr[cnt].note = safe_strdup(note);
    if (!arr[cnt].label || !arr[cnt].note) {
        free(arr[cnt].label); free(arr[cnt].note);
        arr[cnt].label = NULL; arr[cnt].note = NULL;
        return;
    }
    cnt++;
}

static void cmd_delete(long time, char *label) {
    size_t w = 0;
    for (size_t r = 0; r < cnt; r++) {
        if (arr[r].time == time && strcmp(arr[r].label, label) == 0) {
            free(arr[r].label); free(arr[r].note);
        } else {
            arr[w++] = arr[r];
        }
    }
    cnt = w;
}

static void cmd_find(char *label) {
    long best_time = -1;
    size_t best_idx = 0;
    for (size_t i = 0; i < cnt; i++) {
        if (strcmp(arr[i].label, label) == 0) {
            if (best_time < 0 || arr[i].time < best_time) {
                best_time = arr[i].time;
                best_idx = i;
            }
        }
    }
    if (best_time < 0) return;
    printf("%ld %s %s\n", arr[best_idx].time, arr[best_idx].label, arr[best_idx].note);
}

static void cmd_sort(void) {
    if (cnt > 1) qsort(arr, cnt, sizeof(Activity), cmp_act);
    for (size_t i = 0; i < cnt; i++)
        printf("%ld %s %s\n", arr[i].time, arr[i].label, arr[i].note);
}

static void cmd_clear(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].label); free(arr[i].note); }
    cnt = 0;
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) { free(arr[i].label); free(arr[i].note); }
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
            long time_val = 0; char label[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%ld%n %255s%n", &time_val, &n2, label, &n3) < 2) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            cmd_add(time_val, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            long time_val = 0; char label[256] = {0};
            if (sscanf(rest, "%ld %255s", &time_val, label) < 2) continue;
            cmd_delete(time_val, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256] = {0};
            if (sscanf(rest, "%255s", label) < 1) continue;
            cmd_find(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            cmd_sort();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            cmd_clear();
        }
    }
    cleanup();
    return 0;
}