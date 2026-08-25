// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
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

int main() {
    Recipe *recipes = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "RECIPE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    recipes = realloc(recipes, capacity * sizeof(Recipe));
                }
                recipes[count].name = strdup(name);
                recipes[count].ingredients = NULL;
                recipes[count].ing_count = 0;
                recipes[count].ing_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char rname[256], iname[256];
            int n2 = 0;
            if (sscanf(p, "%255s %255s%n", rname, iname, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        if (recipes[i].ing_count >= recipes[i].ing_capacity) {
                            recipes[i].ing_capacity = recipes[i].ing_capacity == 0 ? 4 : recipes[i].ing_capacity * 2;
                            recipes[i].ingredients = realloc(recipes[i].ingredients, recipes[i].ing_capacity * sizeof(Ingredient));
                        }
                        recipes[i].ingredients[recipes[i].ing_count].ing_name = strdup(iname);
                        recipes[i].ingredients[recipes[i].ing_count].note = strdup(p);
                        recipes[i].ing_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char rname[256], iname[256];
            if (sscanf(p, "%255s %255s", rname, iname) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, rname) == 0) {
                        int found_ing = -1;
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            if (strcmp(recipes[i].ingredients[j].ing_name, iname) == 0) {
                                found_ing = j;
                                break;
                            }
                        }
                        if (found_ing != -1) {
                            free(recipes[i].ingredients[found_ing].ing_name);
                            free(recipes[i].ingredients[found_ing].note);
                            for (int j = found_ing; j < recipes[i].ing_count - 1; j++) {
                                recipes[i].ingredients[j] = recipes[i].ingredients[j + 1];
                            }
                            recipes[i].ing_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
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
                        free(recipes[found].ingredients[j].ing_name);
                        free(recipes[found].ingredients[j].note);
                    }
                    free(recipes[found].ingredients);
                    for (int i = found; i < count - 1; i++) {
                        recipes[i] = recipes[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        printf("RECIPE %s\n", recipes[i].name);
                        for (int j = 0; j < recipes[i].ing_count; j++) {
                            printf("ING %s %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                        }
                        break;
                    }
                }
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
