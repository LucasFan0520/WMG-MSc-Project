// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *note;
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
        free(s->items[i].note);
    }
    free(s->items);
    s->items = NULL;
    s->count = 0;
    s->capacity = 0;
}

static void store_copy(Store *dst, const Store *src) {
    store_free(dst);
    store_init(dst);
    for (size_t i = 0; i < src->count; i++) {
        if (dst->count == dst->capacity) {
            size_t newcap = dst->capacity ? dst->capacity * 2 : 4;
            Record *tmp = realloc(dst->items, newcap * sizeof(Record));
            if (!tmp) { store_free(dst); store_init(dst); return; }
            dst->items = tmp;
            dst->capacity = newcap;
        }
        dst->items[dst->count].name = safe_strdup(src->items[i].name);
        dst->items[dst->count].note = safe_strdup(src->items[i].note);
        if (dst->items[dst->count].name && dst->items[dst->count].note)
            dst->count++;
        else {
            free(dst->items[dst->count].name);
            free(dst->items[dst->count].note);
            store_free(dst);
            store_init(dst);
            return;
        }
    }
}

static int store_find(Store *s, const char *name) {
    for (size_t i = 0; i < s->count; i++) {
        if (strcmp(s->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void store_add(Store *s, const char *name, const char *note) {
    if (store_find(s, name) >= 0) return;
    if (s->count == s->capacity) {
        size_t newcap = s->capacity ? s->capacity * 2 : 4;
        Record *tmp = realloc(s->items, newcap * sizeof(Record));
        if (!tmp) return;
        s->items = tmp;
        s->capacity = newcap;
    }
    s->items[s->count].name = safe_strdup(name);
    s->items[s->count].note = safe_strdup(note);
    if (s->items[s->count].name && s->items[s->count].note)
        s->count++;
    else {
        free(s->items[s->count].name);
        free(s->items[s->count].note);
    }
}

static void store_delete(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    free(s->items[idx].name);
    free(s->items[idx].note);
    for (size_t i = idx; i < s->count - 1; i++)
        s->items[i] = s->items[i+1];
    s->count--;
}

static void store_report(Store *s) {
    for (size_t i = 0; i < s->count; i++) {
        printf("%s ", s->items[i].name);
        const char *p = s->items[i].note;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
    }
}

static void load_file(Store *s, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    Store tmp;
    store_init(&tmp);
    char line[MAX_LINE];
    int ok = 1;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char name[256], note[4096];
        char *p = line;
        while (isspace(*p)) p++;
        char *tok = strtok(p, " ");
        if (!tok) { ok = 0; break; }
        strcpy(name, tok);
        tok = strtok(NULL, "");
        if (tok) strcpy(note, tok);
        else note[0] = '\0';
        store_add(&tmp, name, note);
        if (store_find(&tmp, name) < 0) { ok = 0; break; }
    }
    fclose(f);
    if (ok) {
        store_copy(s, &tmp);
    }
    store_free(&tmp);
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
    Store s;
    store_init(&s);
    if (argc > 1) {
        load_file(&s, argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                load_file(&s, argv[1]);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            store_add(&s, a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            store_delete(&s, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            store_report(&s);
        }
    }
    store_free(&s);
    return 0;
}