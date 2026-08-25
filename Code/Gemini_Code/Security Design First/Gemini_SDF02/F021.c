/* F021.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ingredient;
    char *note;
} Ing;

typedef struct {
    char *name;
    Ing *ings;
    size_t i_count;
    size_t i_capacity;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Recipe *temp = realloc(recipes, new_cap * sizeof(Recipe));
                    if (!temp) break;
                    recipes = temp;
                    capacity = new_cap;
                }
                recipes[count].name = strdup(name);
                recipes[count].ings = NULL;
                recipes[count].i_count = 0;
                recipes[count].i_capacity = 0;
                if (recipes[count].name) count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    if (r->i_count >= r->i_capacity) {
                        size_t new_cap = r->i_capacity == 0 ? 4 : r->i_capacity * 2;
                        Ing *temp = realloc(r->ings, new_cap * sizeof(Ing));
                        if (!temp) break;
                        r->ings = temp;
                        r->i_capacity = new_cap;
                    }
                    r->ings[r->i_count].ingredient = strdup(ing_name);
                    r->ings[r->i_count].note = strdup(note);
                    if (r->ings[r->i_count].ingredient && r->ings[r->i_count].note) {
                        r->i_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *ing_name = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    for (size_t j = 0; j < r->i_count; j++) {
                        if (strcmp(r->ings[j].ingredient, ing_name) == 0) {
                            free(r->ings[j].ingredient);
                            free(r->ings[j].note);
                            for (size_t k = j; k < r->i_count - 1; k++) {
                                r->ings[k] = r->ings[k + 1];
                            }
                            r->i_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].i_count; j++) {
                        free(recipes[i].ings[j].ingredient);
                        free(recipes[i].ings[j].note);
                    }
                    free(recipes[i].ings);
                    for (size_t j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("RECIPE %s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].i_count; j++) {
                        printf("ING %s %s\n", recipes[i].ings[j].ingredient, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].i_count; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
