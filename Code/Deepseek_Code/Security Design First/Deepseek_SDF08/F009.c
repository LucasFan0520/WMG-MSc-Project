// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct desc {
    char *text;
    struct desc *next;
} Desc;

typedef struct group {
    char *name;
    Desc *desc_head;
    struct group *next;
} Group;

Group *groups = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Group *find_group(const char *name) {
    Group *g = groups;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

void add_desc(Group *g, const char *text) {
    Desc *d = malloc(sizeof(Desc));
    if (!d) { fprintf(stderr, "Memory error\n"); exit(1); }
    d->text = strdup(text);
    if (!d->text) { fprintf(stderr, "Memory error\n"); exit(1); }
    d->next = g->desc_head;
    g->desc_head = d;
}

void event_group(const char *name, const char *desc) {
    Group *g = find_group(name);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) { fprintf(stderr, "Memory error\n"); exit(1); }
        g->name = strdup(name);
        if (!g->name) { fprintf(stderr, "Memory error\n"); exit(1); }
        g->desc_head = NULL;
        g->next = groups;
        groups = g;
    }
    add_desc(g, desc);
}

void merge_groups(const char *a, const char *b, const char *new) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(new)) return;
    Group *gn = malloc(sizeof(Group));
    if (!gn) { fprintf(stderr, "Memory error\n"); exit(1); }
    gn->name = strdup(new);
    if (!gn->name) { fprintf(stderr, "Memory error\n"); exit(1); }
    gn->desc_head = NULL;
    gn->next = groups;
    groups = gn;
    Desc *d = ga->desc_head;
    while (d) { add_desc(gn, d->text); d = d->next; }
    d = gb->desc_head;
    while (d) { add_desc(gn, d->text); d = d->next; }
}

void delete_group(const char *name) {
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            free(g->name);
            Desc *d = g->desc_head;
            while (d) {
                Desc *next = d->next;
                free(d->text);
                free(d);
                d = next;
            }
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    if (!g) return;
    int cnt = 0;
    Desc *d = g->desc_head;
    while (d) { cnt++; d = d->next; }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s ", g->name);
        Desc *d = g->desc_head;
        int first = 1;
        while (d) {
            if (!first) printf(" ");
            for (char *s = d->text; *s; s++) {
                putchar(*s == ' ' ? '_' : *s);
            }
            first = 0;
            d = d->next;
        }
        putchar('\n');
        g = g->next;
    }
}

void free_all(void) {
    Group *g = groups;
    while (g) {
        Group *nextg = g->next;
        free(g->name);
        Desc *d = g->desc_head;
        while (d) {
            Desc *nextd = d->next;
            free(d->text);
            free(d);
            d = nextd;
        }
        free(g);
        g = nextg;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_groups(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "EVENT") == 0) {
            char *gname = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            event_group(gname, rest);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *a = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *b = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *new = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            merge_groups(a, b, new);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            delete_group(rest);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_group(rest);
        }
    }
    free_all();
    return 0;
}