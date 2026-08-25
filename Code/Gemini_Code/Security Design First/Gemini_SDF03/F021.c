// F021.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    size_t ing_count;
    size_t ing_capacity;
} Recipe;

int main(void) {
    Recipe *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Recipe *tmp = realloc(arr, capacity * sizeof(Recipe));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                if (!arr[count].name) exit(1);
                arr[count].ingredients = NULL;
                arr[count].ing_count = 0;
                arr[count].ing_capacity = 0;
                count++;
            }
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *ing = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                ing = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    Recipe *r = &arr[i];
                    if (r->ing_count >= r->ing_capacity) {
                        r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                        Ingredient *tmp = realloc(r->ingredients, r->ing_capacity * sizeof(Ingredient));
                        if (!tmp) exit(1);
                        r->ingredients = tmp;
                    }
                    r->ingredients[r->ing_count].ingredient = strdup(ing);
                    r->ingredients[r->ing_count].note = strdup(note);
                    if (!r->ingredients[r->ing_count].ingredient || !r->ingredients[r->ing_count].note) exit(1);
                    r->ing_count++;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *ing = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                ing = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    Recipe *r = &arr[i];
                    for (size_t j = 0; j < r->ing_count; j++) {
                        if (strcmp(r->ingredients[j].ingredient, ing) == 0) {
                            free(r->ingredients[j].ingredient);
                            free(r->ingredients[j].note);
                            for (size_t m = j; m < r->ing_count - 1; m++) {
                                r->ingredients[m] = r->ingredients[m + 1];
                            }
                            r->ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].ing_count; j++) {
                        free(arr[i].ingredients[j].ingredient);
                        free(arr[i].ingredients[j].note);
                    }
                    free(arr[i].ingredients);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("Recipe: %s\n", arr[i].name);
                    for (size_t j = 0; j < arr[i].ing_count; j++) {
                        printf("- %s: %s\n", arr[i].ingredients[j].ingredient, arr[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].ing_count; j++) {
            free(arr[i].ingredients[j].ingredient);
            free(arr[i].ingredients[j].note);
        }
        free(arr[i].ingredients);
    }
    free(arr);
    free(line);
    return 0;
}
