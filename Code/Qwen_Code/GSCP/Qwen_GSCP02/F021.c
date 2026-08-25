/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int icount;
    int icap;
} Recipe;

static Recipe *recipes = NULL;
static int rcount = 0;
static int rcap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_recipe(const char *name) {
    for (int i = 0; i < rcount; i++)
        if (strcmp(recipes[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            if (find_recipe(name) >= 0) continue;
            if (rcount >= rcap) {
                rcap = rcap == 0 ? 8 : rcap * 2;
                recipes = realloc(recipes, sizeof(Recipe) * rcap);
            }
            recipes[rcount].name = dup_str(name);
            recipes[rcount].ings = NULL;
            recipes[rcount].icount = 0;
            recipes[rcount].icap = 0;
            rcount++;
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256] = {0}, ing[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, "%255s%n %255s%n", name, &off2, ing, &off3) != 2) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            int idx = find_recipe(name);
            if (idx < 0) continue;
            Recipe *r = &recipes[idx];
            if (r->icount >= r->icap) {
                r->icap = r->icap == 0 ? 4 : r->icap * 2;
                r->ings = realloc(r->ings, sizeof(Ingredient) * r->icap);
            }
            r->ings[r->icount].ingredient = dup_str(ing);
            r->ings[r->icount].note = dup_str(note);
            r->icount++;
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256] = {0}, ing[256] = {0};
            if (sscanf(line + offset, "%255s %255s", name, ing) != 2) continue;
            int idx = find_recipe(name);
            if (idx < 0) continue;
            Recipe *r = &recipes[idx];
            for (int i = 0; i < r->icount; i++) {
                if (strcmp(r->ings[i].ingredient, ing) == 0) {
                    free(r->ings[i].ingredient);
                    free(r->ings[i].note);
                    for (int j = i; j < r->icount - 1; j++) r->ings[j] = r->ings[j + 1];
                    r->icount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_recipe(name);
            if (idx < 0) continue;
            free(recipes[idx].name);
            for (int i = 0; i < recipes[idx].icount; i++) {
                free(recipes[idx].ings[i].ingredient);
                free(recipes[idx].ings[i].note);
            }
            free(recipes[idx].ings);
            for (int i = idx; i < rcount - 1; i++) recipes[i] = recipes[i + 1];
            rcount--;
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_recipe(name);
            if (idx < 0) { printf("NOT_FOUND\n"); continue; }
            for (int i = 0; i < recipes[idx].icount; i++) {
                printf("%s %s\n", recipes[idx].ings[i].ingredient, recipes[idx].ings[i].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < rcount; i++) printf("%s\n", recipes[i].name);
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].icount; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}