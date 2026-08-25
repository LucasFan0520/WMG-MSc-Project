// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
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
    size_t ing_count;
    size_t ing_cap;
} Recipe;

int main(void) {
    Recipe *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Recipe *nb = realloc(list, cap * sizeof(Recipe));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].name = strdup(arg1);
                list[count].ingredients = NULL;
                list[count].ing_count = 0;
                list[count].ing_cap = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *rname = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, rname) == 0) {
                    if (list[i].ing_count >= list[i].ing_cap) {
                        list[i].ing_cap = list[i].ing_cap == 0 ? 4 : list[i].ing_cap * 2;
                        Ingredient *ni = realloc(list[i].ingredients, list[i].ing_cap * sizeof(Ingredient));
                        if (ni) list[i].ingredients = ni;
                    }
                    if (list[i].ing_count < list[i].ing_cap) {
                        list[i].ingredients[list[i].ing_count].ing_name = strdup(ing);
                        list[i].ingredients[list[i].ing_count].note = strdup(note);
                        list[i].ing_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *rname = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, rname) == 0) {
                    for (size_t j = 0; j < list[i].ing_count; j++) {
                        if (strcmp(list[i].ingredients[j].ing_name, ing) == 0) {
                            free(list[i].ingredients[j].ing_name);
                            free(list[i].ingredients[j].note);
                            for (size_t k = j; k < list[i].ing_count - 1; k++) {
                                list[i].ingredients[k] = list[i].ingredients[k + 1];
                            }
                            list[i].ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    for (size_t j = 0; j < list[i].ing_count; j++) {
                        free(list[i].ingredients[j].ing_name);
                        free(list[i].ingredients[j].note);
                    }
                    free(list[i].ingredients);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    printf("Recipe: %s\n", list[i].name);
                    for (size_t j = 0; j < list[i].ing_count; j++) {
                        printf("- %s: %s\n", list[i].ingredients[j].ing_name, list[i].ingredients[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        for (size_t j = 0; j < list[i].ing_count; j++) {
            free(list[i].ingredients[j].ing_name);
            free(list[i].ingredients[j].note);
        }
        free(list[i].ingredients);
    }
    free(list);
    return 0;
}
