// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *key;
    char **values;
    size_t count;
    size_t capacity;
} KeyEntry;

typedef struct {
    KeyEntry *items;
    size_t count;
    size_t capacity;
} Index;

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

static void ke_init(KeyEntry *ke) {
    ke->key = NULL;
    ke->values = NULL;
    ke->count = 0;
    ke->capacity = 0;
}

static void ke_free(KeyEntry *ke) {
    free(ke->key);
    for (size_t i = 0; i < ke->count; i++)
        free(ke->values[i]);
    free(ke->values);
    ke->key = NULL;
    ke->values = NULL;
    ke->count = 0;
    ke->capacity = 0;
}

static void idx_init(Index *idx) {
    idx->items = NULL;
    idx->count = 0;
    idx->capacity = 0;
}

static void idx_free(Index *idx) {
    for (size_t i = 0; i < idx->count; i++)
        ke_free(&idx->items[i]);
    free(idx->items);
    idx->items = NULL;
    idx->count = 0;
    idx->capacity = 0;
}

static int idx_find(Index *idx, const char *key) {
    for (size_t i = 0; i < idx->count; i++) {
        if (strcmp(idx->items[i].key, key) == 0)
            return (int)i;
    }
    return -1;
}

static void idx_add(Index *idx, const char *key, const char *value) {
    int pos = idx_find(idx, key);
    if (pos < 0) {
        if (idx->count == idx->capacity) {
            size_t newcap = idx->capacity ? idx->capacity * 2 : 4;
            KeyEntry *tmp = realloc(idx->items, newcap * sizeof(KeyEntry));
            if (!tmp) return;
            idx->items = tmp;
            idx->capacity = newcap;
        }
        ke_init(&idx->items[idx->count]);
        idx->items[idx->count].key = safe_strdup(key);
        if (!idx->items[idx->count].key) {
            idx->items[idx->count].key = NULL;
            return;
        }
        pos = (int)idx->count;
        idx->count++;
    }
    KeyEntry *ke = &idx->items[pos];
    if (ke->count == ke->capacity) {
        size_t newcap = ke->capacity ? ke->capacity * 2 : 4;
        char **tmp = realloc(ke->values, newcap * sizeof(char *));
        if (!tmp) return;
        ke->values = tmp;
        ke->capacity = newcap;
    }
    ke->values[ke->count] = safe_strdup(value);
    if (ke->values[ke->count])
        ke->count++;
}

static void idx_remove(Index *idx, const char *key, const char *value) {
    int pos = idx_find(idx, key);
    if (pos < 0) return;
    KeyEntry *ke = &idx->items[pos];
    size_t write = 0;
    for (size_t i = 0; i < ke->count; i++) {
        if (strcmp(ke->values[i], value) != 0) {
            if (write != i) ke->values[write] = ke->values[i];
            write++;
        } else {
            free(ke->values[i]);
        }
    }
    ke->count = write;
}

static void idx_erase(Index *idx, const char *key) {
    int pos = idx_find(idx, key);
    if (pos < 0) return;
    ke_free(&idx->items[pos]);
    for (size_t i = pos; i < idx->count - 1; i++)
        idx->items[i] = idx->items[i+1];
    idx->count--;
}

static void idx_values(Index *idx, const char *key) {
    int pos = idx_find(idx, key);
    if (pos < 0) return;
    for (size_t i = 0; i < idx->items[pos].count; i++) {
        printf("%s\n", idx->items[pos].values[i]);
    }
}

static void idx_keys(Index *idx) {
    for (size_t i = 0; i < idx->count; i++) {
        printf("%s\n", idx->items[i].key);
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
    Index idx;
    idx_init(&idx);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ADD") == 0) {
            idx_add(&idx, a1, a2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            idx_remove(&idx, a1, a2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            idx_erase(&idx, a1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            idx_values(&idx, a1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            idx_keys(&idx);
        }
    }
    idx_free(&idx);
    return 0;
}