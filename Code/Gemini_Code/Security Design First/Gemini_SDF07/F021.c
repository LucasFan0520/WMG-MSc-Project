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
    int ing_count;
    int ing_cap;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            char *n_name = strdup(name);
            if (n_name) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Recipe *tmp = realloc(recipes, new_cap * sizeof(Recipe));
                    if (tmp) {
                        recipes = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name);
                        continue;
                    }
                }
                recipes[count].name = n_name;
                recipes[count].ings = NULL;
                recipes[count].ing_count = 0;
                recipes[count].ing_cap = 0;
                count++;
            }
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *args = line + 4;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                char *rname = args;
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                char *ing_name = NULL;
                char *note_text = NULL;
                if (space2) {
                    *space2 = '\0';
                    ing_name = rest;
                    note_text = space2 + 1;
                } else {
                    ing_name = rest;
                    note_text = "";
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        Recipe *r = &recipes[i];
                        char *n_ing = strdup(ing_name);
                        char *n_note = strdup(note_text);
                        if (n_ing && n_note) {
                            if (r->ing_count >= r->ing_cap) {
                                int new_icap = r->ing_cap == 0 ? 4 : r->ing_cap * 2;
                                Ingredient *tmp = realloc(r->ings, new_icap * sizeof(Ingredient));
                                if (tmp) {
                                    r->ings = tmp;
                                    r->ing_cap = new_icap;
                                } else {
                                    free(n_ing);
                                    free(n_note);
                                    break;
                                }
                            }
                            r->ings[r->ing_count].ingredient = n_ing;
                            r->ings[r->ing_count].note = n_note;
                            r->ing_count++;
                        } else {
                            free(n_ing);
                            free(n_note);
                        }
                        break;
                    }
                }
                *space1 = ' ';
                if (space2) *space2 = ' ';
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *args = line + 10;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *rname = args;
                char *ing_name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        Recipe *r = &recipes[i];
                        for (int j = 0; j < r->ing_count; j++) {
                            if (strcmp(r->ings[j].ingredient, ing_name) == 0) {
                                free(r->ings[j].ingredient);
                                free(r->ings[j].note);
                                for (int k = j; k < r->ing_count - 1; k++) {
                                    r->ings[k] = r->ings[k + 1];
                                }
                                r->ing_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *rname = line + 13;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ings[j].ingredient);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    for (int j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *rname = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    printf("%s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("%s %s\n", recipes[i].ings[j].ingredient, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
