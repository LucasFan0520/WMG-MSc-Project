// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *tag;
    char *message;
} Record;

typedef struct {
    Record *items;
    size_t count;
    size_t capacity;
} Collector;

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

static void collector_init(Collector *c) {
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static void collector_free(Collector *c) {
    for (size_t i = 0; i < c->count; i++) {
        free(c->items[i].tag);
        free(c->items[i].message);
    }
    free(c->items);
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static void collector_add(Collector *c, const char *tag, const char *msg) {
    if (c->count == c->capacity) {
        size_t newcap = c->capacity ? c->capacity * 2 : 4;
        Record *tmp = realloc(c->items, newcap * sizeof(Record));
        if (!tmp) return;
        c->items = tmp;
        c->capacity = newcap;
    }
    c->items[c->count].tag = safe_strdup(tag);
    c->items[c->count].message = safe_strdup(msg);
    if (c->items[c->count].tag && c->items[c->count].message)
        c->count++;
    else {
        free(c->items[c->count].tag);
        free(c->items[c->count].message);
    }
}

static void collector_remove(Collector *c, const char *tag) {
    size_t write = 0;
    for (size_t i = 0; i < c->count; i++) {
        if (strcmp(c->items[i].tag, tag) != 0) {
            if (write != i) {
                c->items[write] = c->items[i];
            }
            write++;
        } else {
            free(c->items[i].tag);
            free(c->items[i].message);
        }
    }
    c->count = write;
}

static char *first_message(Collector *c, const char *tag) {
    for (size_t i = 0; i < c->count; i++) {
        if (strcmp(c->items[i].tag, tag) == 0)
            return c->items[i].message;
    }
    return NULL;
}

static void collector_concat(Collector *c, const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = first_message(c, tag1);
    char *m2 = first_message(c, tag2);
    if (!m1 || !m2) return;
    size_t len = strlen(m1) + strlen(m2) + 2;
    char *combined = malloc(len);
    if (!combined) return;
    sprintf(combined, "%s %s", m1, m2);
    collector_add(c, newtag, combined);
    free(combined);
}

static void collector_find(Collector *c, const char *tag) {
    char *m = first_message(c, tag);
    if (!m) {
        printf("NOT_FOUND\n");
        return;
    }
    for (const char *p = m; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void collector_report(Collector *c) {
    for (size_t i = 0; i < c->count; i++) {
        printf("%s: ", c->items[i].tag);
        const char *p = c->items[i].message;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *arg1, size_t a1sz, char *arg2, size_t a2sz, char *arg3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) arg1[i++] = *p++;
    arg1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) arg2[i++] = *p++;
    arg2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) arg3[i++] = *p++;
    arg3[i] = '\0';
}

int main(void) {
    Collector c;
    collector_init(&c);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[4096];
        parse_cmd(line, cmd, sizeof(cmd), arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));
        if (strcmp(cmd, "ADD") == 0) {
            collector_add(&c, arg1, arg2); // arg2 is message if only two args? Actually ADD tag message, so arg1=tag, arg2=message
        } else if (strcmp(cmd, "CONCAT") == 0) {
            collector_concat(&c, arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            collector_remove(&c, arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            collector_find(&c, arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            collector_report(&c);
        }
    }
    collector_free(&c);
    return 0;
}