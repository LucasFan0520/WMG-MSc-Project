// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *item;
    long long count;
} Inv;

typedef struct {
    Inv *items;
    size_t count;
    size_t capacity;
} Inventory;

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

static void inv_init(Inventory *inv) {
    inv->items = NULL;
    inv->count = 0;
    inv->capacity = 0;
}

static void inv_free(Inventory *inv) {
    for (size_t i = 0; i < inv->count; i++)
        free(inv->items[i].item);
    free(inv->items);
    inv->items = NULL;
    inv->count = 0;
    inv->capacity = 0;
}

static int inv_find(Inventory *inv, const char *item) {
    for (size_t i = 0; i < inv->count; i++) {
        if (strcmp(inv->items[i].item, item) == 0)
            return (int)i;
    }
    return -1;
}

static void inv_add(Inventory *inv, const char *item, long long count) {
    int idx = inv_find(inv, item);
    if (idx >= 0) {
        inv->items[idx].count += count;
        return;
    }
    if (inv->count == inv->capacity) {
        size_t newcap = inv->capacity ? inv->capacity * 2 : 4;
        Inv *tmp = realloc(inv->items, newcap * sizeof(Inv));
        if (!tmp) return;
        inv->items = tmp;
        inv->capacity = newcap;
    }
    inv->items[inv->count].item = safe_strdup(item);
    inv->items[inv->count].count = count;
    if (inv->items[inv->count].item)
        inv->count++;
    else
        free(inv->items[inv->count].item);
}

static void inv_take(Inventory *inv, const char *item, long long count) {
    int idx = inv_find(inv, item);
    if (idx < 0) return;
    if (count < inv->items[idx].count)
        inv->items[idx].count -= count;
    else
        inv->items[idx].count = 0;
}

static void inv_remove(Inventory *inv, const char *item) {
    int idx = inv_find(inv, item);
    if (idx < 0) return;
    free(inv->items[idx].item);
    for (size_t i = idx; i < inv->count - 1; i++)
        inv->items[i] = inv->items[i+1];
    inv->count--;
}

static void inv_count(Inventory *inv, const char *item) {
    int idx = inv_find(inv, item);
    printf("%lld\n", idx >= 0 ? inv->items[idx].count : 0);
}

static void inv_report(Inventory *inv) {
    for (size_t i = 0; i < inv->count; i++) {
        printf("%s %lld\n", inv->items[i].item, inv->items[i].count);
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
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
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    Inventory inv;
    inv_init(&inv);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[64];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ADD") == 0) {
            long long cnt = atoll(a2);
            inv_add(&inv, a1, cnt);
        } else if (strcmp(cmd, "TAKE") == 0) {
            long long cnt = atoll(a2);
            inv_take(&inv, a1, cnt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            inv_remove(&inv, a1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            inv_count(&inv, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            inv_report(&inv);
        }
    }
    inv_free(&inv);
    return 0;
}