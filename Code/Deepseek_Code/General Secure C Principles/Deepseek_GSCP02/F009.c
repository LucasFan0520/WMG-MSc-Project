// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Description {
    char *text;
    struct Description *next;
} Description;

typedef struct Group {
    char *name;
    Description *desc_head;
    Description *desc_tail;
    struct Group *next;
} Group;

Group *g_head = NULL;

Group *find_group(const char *name) {
    Group *g = g_head;
    while (g) {
        if (strcmp(g->name, name) == 0) return g;
        g = g->next;
    }
    return NULL;
}

void add_description(Group *g, const char *text) {
    Description *d = malloc(sizeof(Description));
    if (!d) return;
    d->text = strdup(text);
    d->next = NULL;
    if (g->desc_tail) {
        g->desc_tail->next = d;
        g->desc_tail = d;
    } else {
        g->desc_head = g->desc_tail = d;
    }
}

void event_group(const char *name, const char *desc) {
    Group *g = find_group(name);
    if (!g) {
        g = malloc(sizeof(Group));
        if (!g) return;
        g->name = strdup(name);
        g->desc_head = g->desc_tail = NULL;
        g->next = g_head;
        g_head = g;
    }
    add_description(g, desc);
}

void merge_groups(const char *a, const char *b, const char *new) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    if (find_group(new)) return;
    Group *gn = malloc(sizeof(Group));
    if (!gn) return;
    gn->name = strdup(new);
    gn->desc_head = gn->desc_tail = NULL;
    Description *d = ga->desc_head;
    while (d) {
        add_description(gn, d->text);
        d = d->next;
    }
    d = gb->desc_head;
    while (d) {
        add_description(gn, d->text);
        d = d->next;
    }
    gn->next = g_head;
    g_head = gn;
}

void delete_group(const char *name) {
    Group *prev = NULL;
    Group *g = g_head;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else g_head = g->next;
            free(g->name);
            Description *d = g->desc_head;
            while (d) {
                Description *next = d->next;
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
    if (!g) { printf("0\n"); return; }
    int n = 0;
    Description *d = g->desc_head;
    while (d) { n++; d = d->next; }
    printf("%d\n", n);
}

void report_groups(void) {
    Group *g = g_head;
    while (g) {
        printf("%s\n", g->name);
        Description *d = g->desc_head;
        while (d) {
            for (char *p = d->text; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
            d = d->next;
        }
        g = g->next;
    }
}

void free_all(void) {
    while (g_head) {
        Group *next = g_head->next;
        free(g_head->name);
        Description *d = g_head->desc_head;
        while (d) {
            Description *nextd = d->next;
            free(d->text);
            free(d);
            d = nextd;
        }
        free(g_head);
        g_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "EVENT") == 0) {
            char name[256], desc[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, desc) == 2) {
                event_group(name, desc);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char a[256], b[256], new[256];
            if (sscanf(line, "%*s %255s %255s %255s", a, b, new) == 3) {
                merge_groups(a, b, new);
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_group(name);
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                count_group(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }

    free(line);
    free_all();
    return 0;
}