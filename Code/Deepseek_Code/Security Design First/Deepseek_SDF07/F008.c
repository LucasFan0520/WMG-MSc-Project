// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

typedef struct {
    Student *items;
    size_t count;
    size_t capacity;
} Gradebook;

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

static void gb_init(Gradebook *g) {
    g->items = NULL;
    g->count = 0;
    g->capacity = 0;
}

static void gb_free(Gradebook *g) {
    for (size_t i = 0; i < g->count; i++) {
        free(g->items[i].name);
        free(g->items[i].note);
    }
    free(g->items);
    g->items = NULL;
    g->count = 0;
    g->capacity = 0;
}

static int gb_find(Gradebook *g, const char *name) {
    for (size_t i = 0; i < g->count; i++) {
        if (strcmp(g->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void gb_add(Gradebook *g, const char *name, int score, const char *note) {
    if (gb_find(g, name) >= 0) return;
    if (g->count == g->capacity) {
        size_t newcap = g->capacity ? g->capacity * 2 : 4;
        Student *tmp = realloc(g->items, newcap * sizeof(Student));
        if (!tmp) return;
        g->items = tmp;
        g->capacity = newcap;
    }
    g->items[g->count].name = safe_strdup(name);
    g->items[g->count].score = score;
    g->items[g->count].note = safe_strdup(note);
    if (g->items[g->count].name && g->items[g->count].note)
        g->count++;
    else {
        free(g->items[g->count].name);
        free(g->items[g->count].note);
    }
}

static void gb_update(Gradebook *g, const char *name, int score, const char *note) {
    int idx = gb_find(g, name);
    if (idx < 0) return;
    char *newnote = safe_strdup(note);
    if (!newnote) return;
    free(g->items[idx].note);
    g->items[idx].note = newnote;
    g->items[idx].score = score;
}

static void gb_drop(Gradebook *g, const char *name) {
    int idx = gb_find(g, name);
    if (idx < 0) return;
    free(g->items[idx].name);
    free(g->items[idx].note);
    for (size_t i = idx; i < g->count - 1; i++)
        g->items[i] = g->items[i+1];
    g->count--;
}

static void gb_average(Gradebook *g) {
    if (g->count == 0) {
        printf("0\n");
        return;
    }
    long long sum = 0;
    for (size_t i = 0; i < g->count; i++)
        sum += g->items[i].score;
    printf("%lld\n", sum / (long long)g->count);
}

static void gb_list(Gradebook *g) {
    for (size_t i = 0; i < g->count; i++) {
        printf("%s\n", g->items[i].name);
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
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
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Gradebook g;
    gb_init(&g);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[64], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            int score = atoi(a2);
            gb_add(&g, a1, score, a3);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            int score = atoi(a2);
            gb_update(&g, a1, score, a3);
        } else if (strcmp(cmd, "DROP") == 0) {
            gb_drop(&g, a1);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            gb_average(&g);
        } else if (strcmp(cmd, "LIST") == 0) {
            gb_list(&g);
        }
    }
    gb_free(&g);
    return 0;
}