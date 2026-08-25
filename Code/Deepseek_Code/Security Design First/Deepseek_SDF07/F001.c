// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *id;
    char *text;
} Paragraph;

typedef struct {
    Paragraph *items;
    size_t count;
    size_t capacity;
} Notebook;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void notebook_init(Notebook *nb) {
    nb->items = NULL;
    nb->count = 0;
    nb->capacity = 0;
}

static void notebook_free(Notebook *nb) {
    for (size_t i = 0; i < nb->count; i++) {
        free(nb->items[i].id);
        free(nb->items[i].text);
    }
    free(nb->items);
    nb->items = NULL;
    nb->count = 0;
    nb->capacity = 0;
}

static int notebook_find(Notebook *nb, const char *id) {
    for (size_t i = 0; i < nb->count; i++) {
        if (strcmp(nb->items[i].id, id) == 0)
            return (int)i;
    }
    return -1;
}

static void notebook_add(Notebook *nb, const char *id, const char *text) {
    if (notebook_find(nb, id) >= 0) return;
    if (nb->count == nb->capacity) {
        size_t newcap = nb->capacity ? nb->capacity * 2 : 4;
        Paragraph *tmp = realloc(nb->items, newcap * sizeof(Paragraph));
        if (!tmp) return;
        nb->items = tmp;
        nb->capacity = newcap;
    }
    nb->items[nb->count].id = safe_strdup(id);
    nb->items[nb->count].text = safe_strdup(text);
    if (nb->items[nb->count].id && nb->items[nb->count].text)
        nb->count++;
    else {
        free(nb->items[nb->count].id);
        free(nb->items[nb->count].text);
    }
}

static void notebook_append(Notebook *nb, const char *id, const char *text) {
    int idx = notebook_find(nb, id);
    if (idx < 0) return;
    char *old = nb->items[idx].text;
    size_t newlen = strlen(old) + strlen(text) + 1;
    char *newtext = malloc(newlen + 1);
    if (!newtext) return;
    sprintf(newtext, "%s%s", old, text);
    free(old);
    nb->items[idx].text = newtext;
}

static void notebook_replace(Notebook *nb, const char *id, const char *text) {
    int idx = notebook_find(nb, id);
    if (idx < 0) return;
    char *newtext = safe_strdup(text);
    if (!newtext) return;
    free(nb->items[idx].text);
    nb->items[idx].text = newtext;
}

static void notebook_delete(Notebook *nb, const char *id) {
    int idx = notebook_find(nb, id);
    if (idx < 0) return;
    free(nb->items[idx].id);
    free(nb->items[idx].text);
    for (size_t i = idx; i < nb->count - 1; i++) {
        nb->items[i] = nb->items[i+1];
    }
    nb->count--;
}

static void notebook_show(Notebook *nb, const char *id) {
    int idx = notebook_find(nb, id);
    if (idx < 0) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *p = nb->items[idx].text;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
}

static void notebook_list(Notebook *nb) {
    for (size_t i = 0; i < nb->count; i++) {
        printf("%s\n", nb->items[i].id);
    }
}

static void parse_id_text(const char *line, char *cmd, size_t cmdsz,
                          char *id, size_t idsz, char *text, size_t textsz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) {
        cmd[i++] = *p++;
    }
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < idsz - 1) {
        id[i++] = *p++;
    }
    id[i] = '\0';
    while (isspace(*p)) p++;
    size_t j = 0;
    while (*p && j < textsz - 1) {
        text[j++] = *p++;
    }
    text[j] = '\0';
}

int main(void) {
    Notebook nb;
    notebook_init(&nb);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[64], id[256], text[4096];
        parse_id_text(line, cmd, sizeof(cmd), id, sizeof(id), text, sizeof(text));
        if (strcmp(cmd, "NEW") == 0) {
            notebook_add(&nb, id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            notebook_append(&nb, id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            notebook_replace(&nb, id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            notebook_delete(&nb, id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            notebook_show(&nb, id);
        } else if (strcmp(cmd, "LIST") == 0) {
            notebook_list(&nb);
        }
    }
    notebook_free(&nb);
    return 0;
}