// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *key;
    char *value;
} Record;

typedef struct {
    Record *items;
    size_t count;
    size_t capacity;
} Store;

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

static void store_init(Store *s) {
    s->items = NULL;
    s->count = 0;
    s->capacity = 0;
}

static void store_free(Store *s) {
    for (size_t i = 0; i < s->count; i++) {
        free(s->items[i].key);
        free(s->items[i].value);
    }
    free(s->items);
    s->items = NULL;
    s->count = 0;
    s->capacity = 0;
}

static void store_add(Store *s, const char *key, const char *value) {
    if (s->count == s->capacity) {
        size_t newcap = s->capacity ? s->capacity * 2 : 4;
        Record *tmp = realloc(s->items, newcap * sizeof(Record));
        if (!tmp) return;
        s->items = tmp;
        s->capacity = newcap;
    }
    s->items[s->count].key = safe_strdup(key);
    s->items[s->count].value = safe_strdup(value);
    if (s->items[s->count].key && s->items[s->count].value)
        s->count++;
    else {
        free(s->items[s->count].key);
        free(s->items[s->count].value);
    }
}

static void store_dedup(Store *s) {
    size_t write = 0;
    for (size_t i = 0; i < s->count; i++) {
        int first = 1;
        for (size_t j = 0; j < i; j++) {
            if (strcmp(s->items[j].key, s->items[i].key) == 0) {
                first = 0;
                break;
            }
        }
        if (first) {
            if (write != i) s->items[write] = s->items[i];
            write++;
        } else {
            free(s->items[i].key);
            free(s->items[i].value);
        }
    }
    s->count = write;
}

static void store_delete(Store *s, const char *key) {
    size_t write = 0;
    for (size_t i = 0; i < s->count; i++) {
        if (strcmp(s->items[i].key, key) != 0) {
            if (write != i) s->items[write] = s->items[i];
            write++;
        } else {
            free(s->items[i].key);
            free(s->items[i].value);
        }
    }
    s->count = write;
}

static void store_find(Store *s, const char *key) {
    for (size_t i = 0; i < s->count; i++) {
        if (strcmp(s->items[i].key, key) == 0)
            printf("%s\n", s->items[i].value);
    }
}

static void store_report(Store *s) {
    for (size_t i = 0; i < s->count; i++) {
        printf("%s %s\n", s->items[i].key, s->items[i].value);
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
    Store s;
    store_init(&s);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ADD") == 0) {
            store_add(&s, a1, a2);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            store_dedup(&s);
        } else if (strcmp(cmd, "DELETE") == 0) {
            store_delete(&s, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            store_find(&s, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            store_report(&s);
        }
    }
    store_free(&s);
    return 0;
}