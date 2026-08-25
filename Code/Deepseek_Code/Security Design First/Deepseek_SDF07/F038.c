// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *encoded;
} Enc;

typedef struct {
    Enc *items;
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
        free(s->items[i].name);
        free(s->items[i].encoded);
    }
    free(s->items);
    s->items = NULL;
    s->count = 0;
    s->capacity = 0;
}

static int store_find(Store *s, const char *name) {
    for (size_t i = 0; i < s->count; i++) {
        if (strcmp(s->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void store_store(Store *s, const char *name, const char *enc) {
    if (store_find(s, name) >= 0) return;
    if (s->count == s->capacity) {
        size_t newcap = s->capacity ? s->capacity * 2 : 4;
        Enc *tmp = realloc(s->items, newcap * sizeof(Enc));
        if (!tmp) return;
        s->items = tmp;
        s->capacity = newcap;
    }
    s->items[s->count].name = safe_strdup(name);
    s->items[s->count].encoded = safe_strdup(enc);
    if (s->items[s->count].name && s->items[s->count].encoded)
        s->count++;
    else {
        free(s->items[s->count].name);
        free(s->items[s->count].encoded);
    }
}

static void store_decode(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    const char *enc = s->items[idx].encoded;
    while (*enc) {
        if (!isdigit(*enc)) { putchar(*enc); enc++; continue; }
        int count = 0;
        while (isdigit(*enc)) {
            count = count * 10 + (*enc - '0');
            if (count > 1000000) { printf("INVALID\n"); return; }
            enc++;
        }
        if (!*enc) { printf("INVALID\n"); return; }
        char ch = *enc++;
        for (int i = 0; i < count; i++) {
            putchar(ch == ' ' ? '_' : ch);
        }
    }
    putchar('\n');
}

static void store_delete(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    free(s->items[idx].name);
    free(s->items[idx].encoded);
    for (size_t i = idx; i < s->count - 1; i++)
        s->items[i] = s->items[i+1];
    s->count--;
}

static void store_list(Store *s) {
    for (size_t i = 0; i < s->count; i++) {
        printf("%s\n", s->items[i].name);
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
        if (strcmp(cmd, "STORE") == 0) {
            store_store(&s, a1, a2);
        } else if (strcmp(cmd, "DECODE") == 0) {
            store_decode(&s, a1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            store_delete(&s, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            store_list(&s);
        }
    }
    store_free(&s);
    return 0;
}