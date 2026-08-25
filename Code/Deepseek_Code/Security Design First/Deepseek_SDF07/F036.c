// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *data;
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
        free(s->items[i].name);
        free(s->items[i].data);
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

static void store_add(Store *s, const char *name, const char *data) {
    if (store_find(s, name) >= 0) return;
    if (s->count == s->capacity) {
        size_t newcap = s->capacity ? s->capacity * 2 : 4;
        Record *tmp = realloc(s->items, newcap * sizeof(Record));
        if (!tmp) return;
        s->items = tmp;
        s->capacity = newcap;
    }
    s->items[s->count].name = safe_strdup(name);
    s->items[s->count].data = safe_strdup(data);
    if (s->items[s->count].name && s->items[s->count].data)
        s->count++;
    else {
        free(s->items[s->count].name);
        free(s->items[s->count].data);
    }
}

static void store_delete(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    free(s->items[idx].name);
    free(s->items[idx].data);
    for (size_t i = idx; i < s->count - 1; i++)
        s->items[i] = s->items[i+1];
    s->count--;
}

static void store_show(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    const char *p = s->items[idx].data;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
}

static void store_report(Store *s) {
    for (size_t i = 0; i < s->count; i++) {
        printf("%s\n", s->items[i].name);
    }
}

static void parse_add(Store *s, const char *line) {
    char name[256], data[4096];
    char *p = (char *)line;
    while (isspace(*p)) p++;
    char *tok = strtok(p, " ");
    if (!tok) return;
    strcpy(name, tok);
    tok = strtok(NULL, " ");
    if (!tok) return;
    char *colon = strchr(tok, ':');
    if (!colon) return;
    *colon = '\0';
    int len = atoi(tok);
    if (len < 0) return;
    char *datastr = colon + 1;
    if ((int)strlen(datastr) != len) return;
    store_add(s, name, datastr);
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
}

int main(void) {
    Store s;
    store_init(&s);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg[4096];
        parse_cmd(line, cmd, sizeof(cmd), arg, sizeof(arg));
        if (strcmp(cmd, "ADD") == 0) {
            parse_add(&s, line);
        } else if (strcmp(cmd, "DELETE") == 0) {
            store_delete(&s, arg);
        } else if (strcmp(cmd, "SHOW") == 0) {
            store_show(&s, arg);
        } else if (strcmp(cmd, "REPORT") == 0) {
            store_report(&s);
        }
    }
    store_free(&s);
    return 0;
}