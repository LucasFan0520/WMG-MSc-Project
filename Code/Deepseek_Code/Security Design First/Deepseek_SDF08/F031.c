// F031.c
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

static void store_batch(Store *s, int n) {
    char line[MAX_LINE];
    Record *batch = NULL;
    size_t bcount = 0;
    int valid = 1;
    for (int i = 0; i < n; i++) {
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        trim_newline(line);
        char name[256], note[4096];
        char *p = line;
        while (isspace(*p)) p++;
        char *tok = strtok(p, " ");
        if (!tok) { valid = 0; break; }
        strcpy(name, tok);
        tok = strtok(NULL, "");
        if (tok) strcpy(note, tok);
        else note[0] = '\0';
        if (store_find(s, name) >= 0) { valid = 0; break; }
        for (size_t j = 0; j < bcount; j++) {
            if (strcmp(batch[j].name, name) == 0) { valid = 0; break; }
        }
        if (!valid) break;
        Record *tmp = realloc(batch, (bcount+1) * sizeof(Record));
        if (!tmp) { valid = 0; break; }
        batch = tmp;
        batch[bcount].name = safe_strdup(name);
        batch[bcount].note = safe_strdup(note);
        if (!batch[bcount].name || !batch[bcount].note) {
            free(batch[bcount].name);
            free(batch[bcount].note);
            valid = 0;
            break;
        }
        bcount++;
    }
    if (valid) {
        for (size_t i = 0; i < bcount; i++)
            store_add(s, batch[i].name, batch[i].note);
    }
    for (size_t i = 0; i < bcount; i++) {
        free(batch[i].name);
        free(batch[i].note);
    }
    free(batch);
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

static void store_find_print(Store *s, const char *name) {
    int idx = store_find(s, name);
    if (idx < 0) return;
    const char *p = s->items[idx].note;
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
        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(a1);
            store_batch(&s, n);
        } else if (strcmp(cmd, "DELETE") == 0) {
            store_delete(&s, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            store_find_print(&s, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            store_report(&s);
        }
    }
    store_free(&s);
    return 0;
}