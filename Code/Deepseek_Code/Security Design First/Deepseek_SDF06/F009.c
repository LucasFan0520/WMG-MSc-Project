// F009.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **descriptions;
    size_t desc_count;
    size_t desc_cap;
} EventGroup;

typedef struct {
    char *name;
    EventGroup events;
} Group;

static Group *groups = NULL;
static size_t group_count = 0;
static size_t group_cap = 0;

static ssize_t find_group(const char *name) {
    for (size_t i = 0; i < group_count; i++)
        if (strcmp(groups[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_description(EventGroup *eg, const char *desc) {
    if (eg->desc_count == eg->desc_cap) {
        eg->desc_cap = eg->desc_cap ? eg->desc_cap * 2 : 4;
        char **new = realloc(eg->descriptions, eg->desc_cap * sizeof(char *));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        eg->descriptions = new;
    }
    eg->descriptions[eg->desc_count] = strdup(desc);
    if (!eg->descriptions[eg->desc_count]) { fprintf(stderr, "Memory error\n"); exit(1); }
    eg->desc_count++;
}

static void init_group(EventGroup *eg) {
    eg->descriptions = NULL;
    eg->desc_count = 0;
    eg->desc_cap = 0;
}

static void free_group(EventGroup *eg) {
    for (size_t i = 0; i < eg->desc_count; i++)
        free(eg->descriptions[i]);
    free(eg->descriptions);
}

static void add_group(const char *name) {
    if (find_group(name) != -1) return;
    if (group_count == group_cap) {
        group_cap = group_cap ? group_cap * 2 : 4;
        Group *new = realloc(groups, group_cap * sizeof(Group));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        groups = new;
    }
    groups[group_count].name = strdup(name);
    if (!groups[group_count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    init_group(&groups[group_count].events);
    group_count++;
}

static void merge_groups(const char *a, const char *b, const char *newg) {
    ssize_t ia = find_group(a), ib = find_group(b);
    if (ia == -1 || ib == -1) return;
    if (find_group(newg) != -1) return;
    add_group(newg);
    ssize_t inew = find_group(newg);
    if (inew == -1) return;
    for (size_t i = 0; i < groups[ia].events.desc_count; i++)
        add_description(&groups[inew].events, groups[ia].events.descriptions[i]);
    for (size_t i = 0; i < groups[ib].events.desc_count; i++)
        add_description(&groups[inew].events, groups[ib].events.descriptions[i]);
}

static void delete_group(const char *name) {
    ssize_t idx = find_group(name);
    if (idx == -1) return;
    free(groups[idx].name);
    free_group(&groups[idx].events);
    for (size_t i = idx; i < group_count - 1; i++)
        groups[i] = groups[i + 1];
    group_count--;
}

static void count_group(const char *name) {
    ssize_t idx = find_group(name);
    if (idx == -1) { printf("0\n"); return; }
    printf("%zu\n", groups[idx].events.desc_count);
}

static void report(void) {
    for (size_t i = 0; i < group_count; i++) {
        printf("%s", groups[i].name);
        for (size_t j = 0; j < groups[i].events.desc_count; j++)
            printf(" %s", groups[i].events.descriptions[j]);
        putchar('\n');
    }
}

static void free_all(void) {
    for (size_t i = 0; i < group_count; i++) {
        free(groups[i].name);
        free_group(&groups[i].events);
    }
    free(groups);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], g1[256], g2[256], g3[256], desc[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "EVENT") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", g1, desc) == 2) {
                ssize_t idx = find_group(g1);
                if (idx == -1) {
                    add_group(g1);
                    idx = find_group(g1);
                }
                if (idx != -1)
                    add_description(&groups[idx].events, desc);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", g1, g2, g3) == 3)
                merge_groups(g1, g2, g3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            if (sscanf(line, "%*s %255s", g1) == 1)
                delete_group(g1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (sscanf(line, "%*s %255s", g1) == 1)
                count_group(g1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }

    free(line);
    free_all();
    return 0;
}