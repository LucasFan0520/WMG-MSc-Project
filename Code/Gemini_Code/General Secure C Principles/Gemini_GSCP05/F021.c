// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    int ing_count;
    int ing_capacity;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name_start) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Recipe *new_r = realloc(recipes, capacity * sizeof(Recipe));
                    if (new_r) recipes = new_r;
                }
                if (count < capacity) {
                    recipes[count].name = strdup(name_start);
                    recipes[count].ingredients = NULL;
                    recipes[count].ing_count = 0;
                    recipes[count].ing_capacity = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *r_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *ing_name = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(recipes[i].name, r_name) == 0) {
                            Recipe *r = &recipes[i];
                            if (r->ing_count >= r->ing_capacity) {
                                r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                                Ingredient *new_ing = realloc(r->ingredients, r->ing_capacity * sizeof(Ingredient));
                                if (new_ing) r->ingredients = new_ing;
                            }
                            if (r->ing_count < r->ing_capacity) {
                                r->ingredients[r->ing_count].ing_name = strdup(ing_name);
                                r->ingredients[r->ing_count].note = strdup(note);
                                r->ing_count++;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *r_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *ing_name = p + 1;
                while (*ing_name == ' ') ing_name++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, r_name) == 0) {
                        Recipe *r = &recipes[i];
                        for (int j = 0; j < r->ing_count; j++) {
                            if (strcmp(r->ingredients[j].ing_name, ing_name) == 0) {
                                free(r->ingredients[j].ing_name);
                                free(r->ingredients[j].note);
                                for (int k = j; k < r->ing_count - 1; k++) {
                                    r->ingredients[k] = r->ingredients[k + 1];
                                }
                                r->ing_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name_start = line + 13;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name_start) == 0) {
                    free(recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ingredients[j].ing_name);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    for (int j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name_start = line + 5;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name_start) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].ing_count; j++) {
                        printf("  %s: %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].ing_name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
