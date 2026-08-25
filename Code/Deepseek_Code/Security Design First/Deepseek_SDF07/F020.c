// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *title;
    char *body; // whole body with newlines
} Record;

typedef struct {
    Record *items;
    size_t count;
    size_t capacity;
} Catalog;

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

static void cat_init(Catalog *c) {
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static void cat_free(Catalog *c) {
    for (size_t i = 0; i < c->count; i++) {
        free(c->items[i].title);
        free(c->items[i].body);
    }
    free(c->items);
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static int cat_find(Catalog *c, const char *title) {
    for (size_t i = 0; i < c->count; i++) {
        if (strcmp(c->items[i].title, title) == 0)
            return (int)i;
    }
    return -1;
}

static void cat_add(Catalog *c, const char *title, const char *body) {
    if (cat_find(c, title) >= 0) return;
    if (c->count == c->capacity) {
        size_t newcap = c->capacity ? c->capacity * 2 : 4;
        Record *tmp = realloc(c->items, newcap * sizeof(Record));
        if (!tmp) return;
        c->items = tmp;
        c->capacity = newcap;
    }
    c->items[c->count].title = safe_strdup(title);
    c->items[c->count].body = safe_strdup(body);
    if (c->items[c->count].title && c->items[c->count].body)
        c->count++;
    else {
        free(c->items[c->count].title);
        free(c->items[c->count].body);
    }
}

static void cat_delete(Catalog *c, const char *title) {
    int idx = cat_find(c, title);
    if (idx < 0) return;
    free(c->items[idx].title);
    free(c->items[idx].body);
    for (size_t i = idx; i < c->count - 1; i++)
        c->items[i] = c->items[i+1];
    c->count--;
}

static void cat_find_print(Catalog *c, const char *title) {
    int idx = cat_find(c, title);
    if (idx < 0) return;
    printf("%s\n", c->items[idx].title);
    fputs(c->items[idx].body, stdout);
}

static void cat_print(Catalog *c) {
    for (size_t i = 0; i < c->count; i++) {
        printf("%s\n", c->items[i].title);
        fputs(c->items[i].body, stdout);
    }
}

static char *read_until_dash(FILE *f) {
    char *body = NULL;
    size_t total = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strcmp(line, "--") == 0) break;
        size_t len = strlen(line);
        char *new = realloc(body, total + len + 2);
        if (!new) { free(body); return NULL; }
        body = new;
        memcpy(body + total, line, len);
        total += len;
        body[total] = '\n';
        total++;
        body[total] = '\0';
    }
    return body;
}

static void load_file(Catalog *c, const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strcmp(line, "--") == 0) continue;
        char title[MAX_LINE];
        strcpy(title, line);
        char *body = read_until_dash(f);
        if (!body) break;
        cat_add(c, title, body);
        free(body);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    Catalog c;
    cat_init(&c);
    if (argc > 1) {
        load_file(&c, argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (strcmp(line, "FIND") == 0) {
            char title[256];
            fgets(title, sizeof(title), stdin);
            trim_newline(title);
            cat_find_print(&c, title);
        } else if (strcmp(line, "DELETE") == 0) {
            char title[256];
            fgets(title, sizeof(title), stdin);
            trim_newline(title);
            cat_delete(&c, title);
        } else if (strcmp(line, "ADD") == 0) {
            char title[256];
            fgets(title, sizeof(title), stdin);
            trim_newline(title);
            char *body = read_until_dash(stdin);
            if (body) {
                cat_add(&c, title, body);
                free(body);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            cat_print(&c);
        }
    }
    cat_free(&c);
    return 0;
}