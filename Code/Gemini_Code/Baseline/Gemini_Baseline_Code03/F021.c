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
    int ing_count;
} Recipe;

int main() {
    Recipe *recipes = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    recipes = realloc(recipes, (count + 1) * sizeof(Recipe));
                    recipes[count].name = strdup(name);
                    recipes[count].ings = NULL;
                    recipes[count].ing_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char name[256], ing[256];
            int n_len = 0, i_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *ptr2 = ptr + n_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", ing, &i_len) > 0) {
                    char *note = ptr2 + i_len;
                    while (*note == ' ') note++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(recipes[i].name, name) == 0) {
                            int rc = recipes[i].ing_count;
                            recipes[i].ings = realloc(recipes[i].ings, (rc + 1) * sizeof(Ingredient));
                            recipes[i].ings[rc].ingredient = strdup(ing);
                            recipes[i].ings[rc].note = strdup(note);
                            recipes[i].ing_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char name[256], ing[256];
            if (sscanf(ptr, "%255s %255s", name, ing) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        int f_idx = -1;
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ings[j].ingredient, ing) == 0) {
                                f_idx = j;
                                break;
                            }
                        }
                        if (f_idx != -1) {
                            free(recipes[i].ings[f_idx].ingredient);
                            free(recipes[i].ings[f_idx].note);
                            for (int j = f_idx; j < recipes[i].ing_count - 1; j++) {
                                recipes[i].ings[j] = recipes[i].ings[j + 1];
                            }
                            recipes[i].ing_count--;
                            if (recipes[i].ing_count > 0) {
                                recipes[i].ings = realloc(recipes[i].ings, recipes[i].ing_count * sizeof(Ingredient));
                            } else {
                                free(recipes[i].ings);
                                recipes[i].ings = NULL;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(recipes[found].name);
                    for (int j = 0; j < recipes[found].ing_count; j++) {
                        free(recipes[found].ings[j].ingredient);
                        free(recipes[found].ings[j].note);
                    }
                    free(recipes[found].ings);
                    for (int i = found; i < count - 1; i++) {
                        recipes[i] = recipes[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        recipes = realloc(recipes, count * sizeof(Recipe));
                    } else {
                        free(recipes);
                        recipes = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("Recipe: %s\n", recipes[found].name);
                    for (int j = 0; j < recipes[found].ing_count; j++) {
                        printf("  %s: %s\n", recipes[found].ings[j].ingredient, recipes[found].ings[j].note);
                    }
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
    }
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
