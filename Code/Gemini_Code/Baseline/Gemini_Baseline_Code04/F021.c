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
    Ingredient *ingredients;
    int ing_count;
} Recipe;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Recipe *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            arr = realloc(arr, (count + 1) * sizeof(Recipe));
            arr[count].name = strdup(name);
            arr[count].ingredients = NULL;
            arr[count].ing_count = 0;
            count++;
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *ing = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            int ic = arr[i].ing_count;
                            arr[i].ingredients = realloc(arr[i].ingredients, (ic + 1) * sizeof(Ingredient));
                            arr[i].ingredients[ic].ingredient = strdup(ing);
                            arr[i].ingredients[ic].note = strdup(note);
                            arr[i].ing_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *ing = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        for (int j = 0; j < arr[i].ing_count; j++) {
                            if (strcmp(arr[i].ingredients[j].ingredient, ing) == 0) {
                                free(arr[i].ingredients[j].ingredient);
                                free(arr[i].ingredients[j].note);
                                for (int k = j; k < arr[i].ing_count - 1; k++) {
                                    arr[i].ingredients[k] = arr[i].ingredients[k + 1];
                                }
                                arr[i].ing_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (int j = 0; j < arr[i].ing_count; j++) {
                        free(arr[i].ingredients[j].ingredient);
                        free(arr[i].ingredients[j].note);
                    }
                    free(arr[i].ingredients);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("RECIPE %s\n", arr[i].name);
                    for (int j = 0; j < arr[i].ing_count; j++) {
                        printf("ING %s %s\n", arr[i].ingredients[j].ingredient, arr[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].ing_count; j++) {
            free(arr[i].ingredients[j].ingredient);
            free(arr[i].ingredients[j].note);
        }
        free(arr[i].ingredients);
    }
    free(arr);
    return 0;
}
