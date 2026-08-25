// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *ing; char *note; } Ing;
typedef struct {
    char *name;
    Ing *ings;
    size_t icnt, icap;
} Recipe;

static Recipe *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Recipe *t = realloc(arr, nc * sizeof(Recipe));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int ing_grow(Recipe *r) {
    if (r->icnt < r->icap) return 0;
    size_t nc = r->icap ? r->icap * 2 : 4;
    if (nc < r->icap) return -1;
    Ing *t = realloc(r->ings, nc * sizeof(Ing));
    if (!t) return -1;
    r->ings = t; r->icap = nc;
    return 0;
}

static int find_recipe(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_recipe(Recipe *r) {
    free(r->name);
    for (size_t i = 0; i < r->icnt; i++) { free(r->ings[i].ing); free(r->ings[i].note); }
    free(r->ings);
    r->name = NULL; r->ings = NULL; r->icnt = 0; r->icap = 0;
}

static void cmd_recipe(char *name) {
    if (find_recipe(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].ings = NULL; arr[cnt].icnt = 0; arr[cnt].icap = 0;
    if (!arr[cnt].name) return;
    cnt++;
}

static void cmd_ing(char *name, char *ing, char *note) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    Recipe *r = &arr[idx];
    if (ing_grow(r) < 0) return;
    r->ings[r->icnt].ing = safe_strdup(ing);
    r->ings[r->icnt].note = safe_strdup(note);
    if (!r->ings[r->icnt].ing || !r->ings[r->icnt].note) {
        free(r->ings[r->icnt].ing); free(r->ings[r->icnt].note);
        r->ings[r->icnt].ing = NULL; r->ings[r->icnt].note = NULL;
        return;
    }
    r->icnt++;
}

static void cmd_removeing(char *name, char *ing) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    Recipe *r = &arr[idx];
    for (size_t i = 0; i < r->icnt; i++) {
        if (strcmp(r->ings[i].ing, ing) == 0) {
            free(r->ings[i].ing); free(r->ings[i].note);
            for (size_t j = i; j + 1 < r->icnt; j++) r->ings[j] = r->ings[j + 1];
            r->icnt--;
            return;
        }
    }
}

static void cmd_deleterecipe(char *name) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    free_recipe(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_show(char *name) {
    int idx = find_recipe(name);
    if (idx < 0) return;
    printf("%s\n", arr[idx].name);
    for (size_t i = 0; i < arr[idx].icnt; i++)
        printf("%s %s\n", arr[idx].ings[i].ing, arr[idx].ings[i].note);
}

static void cmd_list(void) {
    for (size_t i = 0; i < cnt; i++) printf("%s\n", arr[i].name);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_recipe(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_recipe(name);
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256] = {0}, ing[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%255s%n %255s%n", name, &n2, ing, &n3) < 2) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            cmd_ing(name, ing, note);
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256] = {0}, ing[256] = {0};
            if (sscanf(rest, "%255s %255s", name, ing) < 2) continue;
            cmd_removeing(name, ing);
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_deleterecipe(name);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_show(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}