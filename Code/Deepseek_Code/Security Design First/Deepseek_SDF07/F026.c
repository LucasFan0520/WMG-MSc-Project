// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *category;
    char *text;
} Note;

typedef struct {
    Note *items;
    size_t count;
    size_t capacity;
} NoteList;

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

static void nl_init(NoteList *nl) {
    nl->items = NULL;
    nl->count = 0;
    nl->capacity = 0;
}

static void nl_free(NoteList *nl) {
    for (size_t i = 0; i < nl->count; i++) {
        free(nl->items[i].category);
        free(nl->items[i].text);
    }
    free(nl->items);
    nl->items = NULL;
    nl->count = 0;
    nl->capacity = 0;
}

static void nl_add(NoteList *nl, const char *category, const char *text) {
    if (nl->count == nl->capacity) {
        size_t newcap = nl->capacity ? nl->capacity * 2 : 4;
        Note *tmp = realloc(nl->items, newcap * sizeof(Note));
        if (!tmp) return;
        nl->items = tmp;
        nl->capacity = newcap;
    }
    nl->items[nl->count].category = safe_strdup(category);
    nl->items[nl->count].text = safe_strdup(text);
    if (nl->items[nl->count].category && nl->items[nl->count].text)
        nl->count++;
    else {
        free(nl->items[nl->count].category);
        free(nl->items[nl->count].text);
    }
}

static void nl_delete_category(NoteList *nl, const char *category) {
    size_t write = 0;
    for (size_t i = 0; i < nl->count; i++) {
        if (strcmp(nl->items[i].category, category) != 0) {
            if (write != i) nl->items[write] = nl->items[i];
            write++;
        } else {
            free(nl->items[i].category);
            free(nl->items[i].text);
        }
    }
    nl->count = write;
}

static void nl_keep(NoteList *nl, const char *category) {
    size_t write = 0;
    for (size_t i = 0; i < nl->count; i++) {
        if (strcmp(nl->items[i].category, category) == 0) {
            if (write != i) nl->items[write] = nl->items[i];
            write++;
        } else {
            free(nl->items[i].category);
            free(nl->items[i].text);
        }
    }
    nl->count = write;
}

static void nl_find(NoteList *nl, const char *word) {
    for (size_t i = 0; i < nl->count; i++) {
        if (strstr(nl->items[i].text, word)) {
            const char *p = nl->items[i].text;
            while (*p) {
                putchar(*p == ' ' ? '_' : *p);
                p++;
            }
            putchar('\n');
        }
    }
}

static void nl_print(NoteList *nl) {
    for (size_t i = 0; i < nl->count; i++) {
        printf("%s ", nl->items[i].category);
        const char *p = nl->items[i].text;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
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
    NoteList nl;
    nl_init(&nl);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ADD") == 0) {
            nl_add(&nl, a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            nl_delete_category(&nl, a1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            nl_keep(&nl, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            nl_find(&nl, a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            nl_print(&nl);
        }
    }
    nl_free(&nl);
    return 0;
}