// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *key;
    char *value;
} Config;

typedef struct {
    Config *items;
    size_t count;
    size_t capacity;
} ConfigStore;

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

static void cs_init(ConfigStore *cs) {
    cs->items = NULL;
    cs->count = 0;
    cs->capacity = 0;
}

static void cs_free(ConfigStore *cs) {
    for (size_t i = 0; i < cs->count; i++) {
        free(cs->items[i].key);
        free(cs->items[i].value);
    }
    free(cs->items);
    cs->items = NULL;
    cs->count = 0;
    cs->capacity = 0;
}

static int cs_find(ConfigStore *cs, const char *key) {
    for (size_t i = 0; i < cs->count; i++) {
        if (strcmp(cs->items[i].key, key) == 0)
            return (int)i;
    }
    return -1;
}

static void cs_set(ConfigStore *cs, const char *key, const char *value) {
    int idx = cs_find(cs, key);
    if (idx >= 0) {
        char *new = safe_strdup(value);
        if (!new) return;
        free(cs->items[idx].value);
        cs->items[idx].value = new;
        return;
    }
    if (cs->count == cs->capacity) {
        size_t newcap = cs->capacity ? cs->capacity * 2 : 4;
        Config *tmp = realloc(cs->items, newcap * sizeof(Config));
        if (!tmp) return;
        cs->items = tmp;
        cs->capacity = newcap;
    }
    cs->items[cs->count].key = safe_strdup(key);
    cs->items[cs->count].value = safe_strdup(value);
    if (cs->items[cs->count].key && cs->items[cs->count].value)
        cs->count++;
    else {
        free(cs->items[cs->count].key);
        free(cs->items[cs->count].value);
    }
}

static void cs_unset(ConfigStore *cs, const char *key) {
    int idx = cs_find(cs, key);
    if (idx < 0) return;
    free(cs->items[idx].key);
    free(cs->items[idx].value);
    for (size_t i = idx; i < cs->count - 1; i++)
        cs->items[i] = cs->items[i+1];
    cs->count--;
}

static void cs_get(ConfigStore *cs, const char *key) {
    int idx = cs_find(cs, key);
    if (idx < 0) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", cs->items[idx].value);
}

static void cs_write(ConfigStore *cs) {
    for (size_t i = 0; i < cs->count; i++) {
        printf("%s=%s\n", cs->items[i].key, cs->items[i].value);
    }
}

static void load_file(ConfigStore *cs, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        cs_set(cs, key, val);
    }
    fclose(f);
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

int main(int argc, char **argv) {
    ConfigStore cs;
    cs_init(&cs);
    if (argc > 1) {
        load_file(&cs, argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "SET") == 0) {
            cs_set(&cs, a1, a2);
        } else if (strcmp(cmd, "UNSET") == 0) {
            cs_unset(&cs, a1);
        } else if (strcmp(cmd, "GET") == 0) {
            cs_get(&cs, a1);
        } else if (strcmp(cmd, "WRITE") == 0) {
            cs_write(&cs);
        }
    }
    cs_free(&cs);
    return 0;
}