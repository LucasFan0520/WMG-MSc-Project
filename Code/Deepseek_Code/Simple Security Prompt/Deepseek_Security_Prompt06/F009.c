// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Desc {
    char *text;
    struct Desc *next;
} Desc;

typedef struct Group {
    char *name;
    Desc *descs;
    struct Group *next;
} Group;

Group *groups = NULL;

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
    d->text = malloc(strlen(text) + 1);
    strcpy(d->text, text);
    d->next = g->descs;
    g->descs = d;
}

void event_group(const char *name, const char *desc) {
    Group *g = find_group(name);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = malloc(strlen(name) + 1);
        strcpy(g->name, name);
        g->descs = NULL;
        g->next = groups;
        groups = g;
    }
    add_desc(g, desc);
}

void merge_groups(const char *a, const char *b, const char *newg) {
    Group *ga = find_group(a);
    Group *gb = find_group(b);
    if (!ga || !gb) return;
    Group *g = find_group(newg);
    if (!g) {
        g = malloc(sizeof(Group));
        g->name = malloc(strlen(newg) + 1);
        strcpy(g->name, newg);
        g->descs = NULL;
        g->next = groups;
        groups = g;
    }
    Desc *d = ga->descs;
    while (d) {
        add_desc(g, d->text);
        d = d->next;
    }
    d = gb->descs;
    while (d) {
        add_desc(g, d->text);
        d = d->next;
    }
}

void delete_group(const char *name) {
    Group *g = groups, *prev = NULL;
    while (g) {
        if (strcmp(g->name, name) == 0) {
            if (prev) prev->next = g->next;
            else groups = g->next;
            Desc *d = g->descs;
            while (d) {
                Desc *tmp = d->next;
                free(d->text);
                free(d);
                d = tmp;
            }
            free(g->name);
            free(g);
            return;
        }
        prev = g;
        g = g->next;
    }
}

void count_group(const char *name) {
    Group *g = find_group(name);
    int cnt = 0;
    if (g) {
        Desc *d = g->descs;
        while (d) { cnt++; d = d->next; }
    }
    printf("%d\n", cnt);
}

void report_groups(void) {
    Group *g = groups;
    while (g) {
        printf("%s\n", g->name);
        Desc *d = g->descs;
        while (d) {
            printf("  %s\n", d->text);
            d = d->next;
        }
        g = g->next;
    }
}

void free_all(void) {
    Group *g = groups;
    while (g) {
        Group *tmp = g->next;
        Desc *d = g->descs;
        while (d) {
            Desc *td = d->next;
            free(d->text);
            free(d);
            d = td;
        }
        free(g->name);
        free(g);
        g = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[256];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(rest, "%255s", a) == 1) {
                const char *desc = rest + strlen(a) + 1;
                while (*desc == ' ') desc++;
                if (*desc) event_group(a, desc);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(rest, "%255s %255s %255s", a, b, c) == 3) {
                merge_groups(a, b, c);
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(rest, "%255s", a) == 1) delete_group(a);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(rest, "%255s", a) == 1) count_group(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_groups();
        }
    }
    free_all();
    return 0;
}