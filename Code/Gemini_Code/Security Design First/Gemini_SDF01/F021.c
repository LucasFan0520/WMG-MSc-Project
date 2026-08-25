// F021.c
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
    size_t ing_count;
    size_t ing_capacity;
} Recipe;

int main(void) {
    Recipe *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Recipe *new_items = realloc(items, capacity * sizeof(Recipe));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                if (!items[count].name) exit(1);
                items[count].ings = NULL;
                items[count].ing_count = 0;
                items[count].ing_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *ingredient = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Recipe *r = &items[i];
                    if (r->ing_count >= r->ing_capacity) {
                        r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                        Ingredient *new_ings = realloc(r->ings, r->ing_capacity * sizeof(Ingredient));
                        if (!new_ings) exit(1);
                        r->ings = new_ings;
                    }
                    r->ings[r->ing_count].ingredient = strdup(ingredient);
                    r->ings[r->ing_count].note = strdup(note);
                    if (!r->ings[r->ing_count].ingredient || !r->ings[r->ing_count].note) exit(1);
                    r->ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *ingredient = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Recipe *r = &items[i];
                    for (size_t j = 0; j < r->ing_count; j++) {
                        if (strcmp(r->ings[j].ingredient, ingredient) == 0) {
                            free(r->ings[j].ingredient);
                            free(r->ings[j].note);
                            for (size_t k = j; k < r->ing_count - 1; k++) {
                                r->ings[k] = r->ings[k + 1];
                            }
                            r->ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (size_t j = 0; j < items[i].ing_count; j++) {
                        free(items[i].ings[j].ingredient);
                        free(items[i].ings[j].note);
                    }
                    free(items[i].ings);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("Recipe: %s\n", items[i].name);
                    for (size_t j = 0; j < items[i].ing_count; j++) {
                        printf("  %s: %s\n", items[i].ings[j].ingredient, items[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (size_t j = 0; j < items[i].ing_count; j++) {
            free(items[i].ings[j].ingredient);
            free(items[i].ings[j].note);
        }
        free(items[i].ings);
    }
    free(items);
    free(line);
    return 0;
}
