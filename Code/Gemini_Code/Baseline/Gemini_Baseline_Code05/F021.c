// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int ing_count;
    int ing_capacity;
} Recipe;

Recipe *recipes = NULL;
int r_count = 0;
int r_capacity = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < r_count; i++) {
        if (strcmp(recipes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_recipe(const char *name) {
    if (find_recipe(name) != -1) {
        return;
    }
    if (r_count >= r_capacity) {
        r_capacity = r_capacity == 0 ? 4 : r_capacity * 2;
        recipes = realloc(recipes, r_capacity * sizeof(Recipe));
    }
    recipes[r_count].name = strdup(name);
    recipes[r_count].ings = NULL;
    recipes[r_count].ing_count = 0;
    recipes[r_count].ing_capacity = 0;
    r_count++;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            add_recipe(line + 7);
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p1 = line + 4;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *note = sp2 + 1;
                    int r_idx = find_recipe(p1);
                    if (r_idx != -1) {
                        Recipe *r = &recipes[r_idx];
                        if (r->ing_count >= r->ing_capacity) {
                            r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                            r->ings = realloc(r->ings, r->ing_capacity * sizeof(Ingredient));
                        }
                        r->ings[r->ing_count].ing_name = strdup(p2);
                        r->ings[r->ing_count].note = strdup(note);
                        r->ing_count++;
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p1 = line + 10;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *ing_name = sp + 1;
                int r_idx = find_recipe(p1);
                if (r_idx != -1) {
                    Recipe *r = &recipes[r_idx];
                    for (int i = 0; i < r->ing_count; i++) {
                        if (strcmp(r->ings[i].ing_name, ing_name) == 0) {
                            free(r->ings[i].ing_name);
                            free(r->ings[i].note);
                            for (int j = i; j < r->ing_count - 1; j++) {
                                r->ings[j] = r->ings[j + 1];
                            }
                            r->ing_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            int r_idx = find_recipe(name);
            if (r_idx != -1) {
                free(recipes[r_idx].name);
                for (int i = 0; i < recipes[r_idx].ing_count; i++) {
                    free(recipes[r_idx].ings[i].ing_name);
                    free(recipes[r_idx].ings[i].note);
                }
                free(recipes[r_idx].ings);
                for (int i = r_idx; i < r_count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                r_count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            int r_idx = find_recipe(name);
            if (r_idx != -1) {
                printf("RECIPE %s\n", recipes[r_idx].name);
                for (int i = 0; i < recipes[r_idx].ing_count; i++) {
                    printf("ING %s %s\n", recipes[r_idx].ings[i].ing_name, recipes[r_idx].ings[i].note);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    for (int i = 0; i < r_count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].ing_name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
