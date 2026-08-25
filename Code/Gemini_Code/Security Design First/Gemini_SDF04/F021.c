// F021.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    int ing_count;
    int ing_cap;
} Recipe;

Recipe *recipes = NULL;
int rec_count = 0;
int rec_cap = 0;

int find_recipe(const char *name) {
    for (int i = 0; i < rec_count; i++) {
        if (strcmp(recipes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "RECIPE") == 0) {
                if (find_recipe(arg1) == -1) {
                    if (rec_count >= rec_cap) {
                        rec_cap = rec_cap == 0 ? 4 : rec_cap * 2;
                        Recipe *tmp = realloc(recipes, rec_cap * sizeof(Recipe));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        recipes = tmp;
                    }
                    recipes[rec_count].name = strdup(arg1);
                    recipes[rec_count].ingredients = NULL;
                    recipes[rec_count].ing_count = 0;
                    recipes[rec_count].ing_cap = 0;
                    if (recipes[rec_count].name) {
                        rec_count++;
                    }
                }
            } else if (strcmp(cmd, "ING") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *ing_name = space2 + 1;
                    char *space3 = strchr(ing_name, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *note = space3 + 1;
                        int idx = find_recipe(arg1);
                        if (idx != -1) {
                            Recipe *r = &recipes[idx];
                            if (r->ing_count >= r->ing_cap) {
                                r->ing_cap = r->ing_cap == 0 ? 4 : r->ing_cap * 2;
                                Ingredient *tmp = realloc(r->ingredients, r->ing_cap * sizeof(Ingredient));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                r->ingredients = tmp;
                            }
                            r->ingredients[r->ing_count].name = strdup(ing_name);
                            r->ingredients[r->ing_count].note = strdup(note);
                            if (r->ingredients[r->ing_count].name && r->ingredients[r->ing_count].note) {
                                r->ing_count++;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "REMOVEING") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *ing_name = space2 + 1;
                    int idx = find_recipe(arg1);
                    if (idx != -1) {
                        Recipe *r = &recipes[idx];
                        for (int i = 0; i < r->ing_count; i++) {
                            if (strcmp(r->ingredients[i].name, ing_name) == 0) {
                                free(r->ingredients[i].name);
                                free(r->ingredients[i].note);
                                for (int j = i; j < r->ing_count - 1; j++) {
                                    r->ingredients[j] = r->ingredients[j + 1];
                                }
                                r->ing_count--;
                                break;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETERECIPE") == 0) {
                int idx = find_recipe(arg1);
                if (idx != -1) {
                    free(recipes[idx].name);
                    for (int i = 0; i < recipes[idx].ing_count; i++) {
                        free(recipes[idx].ingredients[i].name);
                        free(recipes[idx].ingredients[i].note);
                    }
                    free(recipes[idx].ingredients);
                    for (int i = idx; i < rec_count - 1; i++) {
                        recipes[i] = recipes[i + 1];
                    }
                    rec_count--;
                }
            } else if (strcmp(cmd, "SHOW") == 0) {
                int idx = find_recipe(arg1);
                if (idx != -1) {
                    printf("Recipe %s:\n", recipes[idx].name);
                    for (int i = 0; i < recipes[idx].ing_count; i++) {
                        printf("  %s: %s\n", recipes[idx].ingredients[i].name, recipes[idx].ingredients[i].note);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < rec_count; i++) {
                    printf("%s\n", recipes[i].name);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < rec_count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}