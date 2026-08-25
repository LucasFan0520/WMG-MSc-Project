// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Ingredient {
    char *name;
    char *note;
    struct Ingredient *next;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *head;
    size_t count;
} Recipe;

typedef struct {
    Recipe *items;
    size_t count;
    size_t capacity;
} Cookbook;

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

static void ing_free(Ingredient *head) {
    while (head) {
        Ingredient *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
}

static void recipe_free(Recipe *r) {
    free(r->name);
    ing_free(r->head);
    r->head = NULL;
    r->count = 0;
}

static void cb_init(Cookbook *cb) {
    cb->items = NULL;
    cb->count = 0;
    cb->capacity = 0;
}

static void cb_free(Cookbook *cb) {
    for (size_t i = 0; i < cb->count; i++)
        recipe_free(&cb->items[i]);
    free(cb->items);
    cb->items = NULL;
    cb->count = 0;
    cb->capacity = 0;
}

static int cb_find(Cookbook *cb, const char *name) {
    for (size_t i = 0; i < cb->count; i++) {
        if (strcmp(cb->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void cb_recipe(Cookbook *cb, const char *name) {
    if (cb_find(cb, name) >= 0) return;
    if (cb->count == cb->capacity) {
        size_t newcap = cb->capacity ? cb->capacity * 2 : 4;
        Recipe *tmp = realloc(cb->items, newcap * sizeof(Recipe));
        if (!tmp) return;
        cb->items = tmp;
        cb->capacity = newcap;
    }
    cb->items[cb->count].name = safe_strdup(name);
    cb->items[cb->count].head = NULL;
    cb->items[cb->count].count = 0;
    if (cb->items[cb->count].name)
        cb->count++;
    else
        free(cb->items[cb->count].name);
}

static Ingredient *cb_find_ing(Cookbook *cb, int idx, const char *ing) {
    Ingredient *cur = cb->items[idx].head;
    while (cur) {
        if (strcmp(cur->name, ing) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void cb_ing(Cookbook *cb, const char *name, const char *ing, const char *note) {
    int idx = cb_find(cb, name);
    if (idx < 0) return;
    if (cb_find_ing(cb, idx, ing)) return;
    Ingredient *new = malloc(sizeof(Ingredient));
    if (!new) return;
    new->name = safe_strdup(ing);
    new->note = safe_strdup(note);
    new->next = cb->items[idx].head;
    if (new->name && new->note) {
        cb->items[idx].head = new;
        cb->items[idx].count++;
    } else {
        free(new->name);
        free(new->note);
        free(new);
    }
}

static void cb_removeing(Cookbook *cb, const char *name, const char *ing) {
    int idx = cb_find(cb, name);
    if (idx < 0) return;
    Ingredient **ptr = &cb->items[idx].head;
    while (*ptr) {
        if (strcmp((*ptr)->name, ing) == 0) {
            Ingredient *del = *ptr;
            *ptr = del->next;
            free(del->name);
            free(del->note);
            free(del);
            cb->items[idx].count--;
            return;
        }
        ptr = &(*ptr)->next;
    }
}

static void cb_deleterecipe(Cookbook *cb, const char *name) {
    int idx = cb_find(cb, name);
    if (idx < 0) return;
    recipe_free(&cb->items[idx]);
    for (size_t i = idx; i < cb->count - 1; i++)
        cb->items[i] = cb->items[i+1];
    cb->count--;
}

static void cb_show(Cookbook *cb, const char *name) {
    int idx = cb_find(cb, name);
    if (idx < 0) return;
    printf("%s\n", cb->items[idx].name);
    Ingredient *cur = cb->items[idx].head;
    while (cur) {
        const char *p = cur->note;
        printf("%s ", cur->name);
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
        cur = cur->next;
    }
}

static void cb_list(Cookbook *cb) {
    for (size_t i = 0; i < cb->count; i++) {
        printf("%s\n", cb->items[i].name);
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
    Cookbook cb;
    cb_init(&cb);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "RECIPE") == 0) {
            cb_recipe(&cb, a1);
        } else if (strcmp(cmd, "ING") == 0) {
            cb_ing(&cb, a1, a2, a3);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            cb_removeing(&cb, a1, a2);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            cb_deleterecipe(&cb, a1);
        } else if (strcmp(cmd, "SHOW") == 0) {
            cb_show(&cb, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            cb_list(&cb);
        }
    }
    cb_free(&cb);
    return 0;
}