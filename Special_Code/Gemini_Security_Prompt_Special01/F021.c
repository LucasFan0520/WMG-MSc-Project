// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ingredients;
    size_t ing_count;
    size_t ing_capacity;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
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
                recipes[count].name = mystrdup(name);
                recipes[count].ingredients = NULL;
                recipes[count].ing_count = 0;
                recipes[count].ing_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *ing = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                ing++;
            }
            while (*ing == ' ') ing++;
            char *ing_end = ing;
            while (*ing_end && *ing_end != ' ') ing_end++;
            char *note = ing_end;
            if (*ing_end != '\0') {
                *ing_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    if (r->ing_count >= r->ing_capacity) {
                        r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                        Ingredient *new_ing = realloc(r->ingredients, r->ing_capacity * sizeof(Ingredient));
                        if (new_ing) r->ingredients = new_ing;
                    }
                    r->ingredients[r->ing_count].ing_name = mystrdup(ing);
                    r->ingredients[r->ing_count].note = mystrdup(note);
                    r->ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *ing = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                ing++;
            }
            while (*ing == ' ') ing++;
            char *ing_end = ing;
            while (*ing_end && *ing_end != ' ') ing_end++;
            *ing_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    Recipe *r = &recipes[i];
                    for (size_t j = 0; j < r->ing_count; j++) {
                        if (strcmp(r->ingredients[j].ing_name, ing) == 0) {
                            free(r->ingredients[j].ing_name);
                            free(r->ingredients[j].note);
                            for (size_t k = j; k < r->ing_count - 1; k++) {
                                r->ingredients[k] = r->ingredients[k + 1];
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
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ingredients[j].ing_name);
                        free(recipes[i].ingredients[j].note);
                    }
                    free(recipes[i].ingredients);
                    for (size_t j = i; j < count - 1; j++) {
                        recipes[j] = recipes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("RECIPE %s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        printf("ING %s %s\n", recipes[i].ingredients[j].ing_name, recipes[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].ing_count; j++) {
            free(recipes[i].ingredients[j].ing_name);
            free(recipes[i].ingredients[j].note);
        }
        free(recipes[i].ingredients);
    }
    free(recipes);
    return 0;
}
