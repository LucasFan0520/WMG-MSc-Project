// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

typedef struct {
    Activity *items;
    size_t count;
    size_t capacity;
} Log;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void log_init(Log *l) {
    l->items = NULL;
    l->count = 0;
    l->capacity = 0;
}

static void log_free(Log *l) {
    for (size_t i = 0; i < l->count; i++) {
        free(l->items[i].time);
        free(l->items[i].label);
        free(l->items[i].note);
    }
    free(l->items);
    l->items = NULL;
    l->count = 0;
    l->capacity = 0;
}

static void log_add(Log *l, const char *time, const char *label, const char *note) {
    if (l->count == l->capacity) {
        size_t newcap = l->capacity ? l->capacity * 2 : 4;
        Activity *tmp = realloc(l->items, newcap * sizeof(Activity));
        if (!tmp) return;
        l->items = tmp;
        l->capacity = newcap;
    }
    l->items[l->count].time = safe_strdup(time);
    l->items[l->count].label = safe_strdup(label);
    l->items[l->count].note = safe_strdup(note);
    if (l->items[l->count].time && l->items[l->count].label && l->items[l->count].note)
        l->count++;
    else {
        free(l->items[l->count].time);
        free(l->items[l->count].label);
        free(l->items[l->count].note);
    }
}

static void log_delete(Log *l, const char *time, const char *label) {
    size_t write = 0;
    for (size_t i = 0; i < l->count; i++) {
        if (strcmp(l->items[i].time, time) != 0 || strcmp(l->items[i].label, label) != 0) {
            if (write != i) l->items[write] = l->items[i];
            write++;
        } else {
            free(l->items[i].time);
            free(l->items[i].label);
            free(l->items[i].note);
        }
    }
    l->count = write;
}

static void log_find(Log *l, const char *label) {
    int earliest = -1;
    for (size_t i = 0; i < l->count; i++) {
        if (strcmp(l->items[i].label, label) == 0) {
            if (earliest < 0 ||
                strcmp(l->items[i].time, l->items[earliest].time) < 0 ||
                (strcmp(l->items[i].time, l->items[earliest].time) == 0 &&
                 strcmp(l->items[i].label, l->items[earliest].label) < 0)) {
                earliest = (int)i;
            }
        }
    }
    if (earliest < 0) return;
    printf("%s %s ", l->items[earliest].time, l->items[earliest].label);
    const char *p = l->items[earliest].note;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
}

static int cmp_activity(const void *a, const void *b) {
    const Activity *pa = (const Activity *)a;
    const Activity *pb = (const Activity *)b;
    int c = strcmp(pa->time, pb->time);
    if (c != 0) return c;
    return strcmp(pa->label, pb->label);
}

static void log_sort(Log *l) {
    Activity *copy = malloc(l->count * sizeof(Activity));
    if (!copy) return;
    for (size_t i = 0; i < l->count; i++) {
        copy[i].time = safe_strdup(l->items[i].time);
        copy[i].label = safe_strdup(l->items[i].label);
        copy[i].note = safe_strdup(l->items[i].note);
        if (!copy[i].time || !copy[i].label || !copy[i].note) {
            for (size_t j = 0; j <= i; j++) {
                free(copy[j].time);
                free(copy[j].label);
                free(copy[j].note);
            }
            free(copy);
            return;
        }
    }
    qsort(copy, l->count, sizeof(Activity), cmp_activity);
    for (size_t i = 0; i < l->count; i++) {
        printf("%s %s ", copy[i].time, copy[i].label);
        const char *p = copy[i].note;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
        free(copy[i].time);
        free(copy[i].label);
        free(copy[i].note);
    }
    free(copy);
}

static void log_clear(Log *l) {
    log_free(l);
    log_init(l);
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Log l;
    log_init(&l);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            log_add(&l, a1, a2, a3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            log_delete(&l, a1, a2);
        } else if (strcmp(cmd, "FIND") == 0) {
            log_find(&l, a1);
        } else if (strcmp(cmd, "SORT") == 0) {
            log_sort(&l);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            log_clear(&l);
        }
    }
    log_free(&l);
    return 0;
}