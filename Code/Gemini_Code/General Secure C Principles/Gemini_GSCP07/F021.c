// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    size_t ing_count;
    size_t ing_capacity;
} Recipe;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Recipe *recipes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
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
                    Recipe *nr = realloc(recipes, capacity * sizeof(Recipe));
                    if (!nr) break;
                    recipes = nr;
                }
                recipes[count].name = strdup(name);
                recipes[count].ings = NULL;
                recipes[count].ing_count = 0;
                recipes[count].ing_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ') p++;
            char *rname = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    Recipe *r = &recipes[i];
                    if (r->ing_count >= r->ing_capacity) {
                        r->ing_capacity = r->ing_capacity == 0 ? 4 : r->ing_capacity * 2;
                        Ingredient *ni = realloc(r->ings, r->ing_capacity * sizeof(Ingredient));
                        if (!ni) break;
                        r->ings = ni;
                    }
                    r->ings[r->ing_count].ing_name = strdup(ing_name);
                    r->ings[r->ing_count].note = strdup(note);
                    r->ing_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ') p++;
            char *rname = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *ing_name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, rname) == 0) {
                    Recipe *r = &recipes[i];
                    for (size_t j = 0; j < r->ing_count; j++) {
                        if (strcmp(r->ings[j].ing_name, ing_name) == 0) {
                            free(r->ings[j].ing_name);
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
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    free(recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        free(recipes[i].ings[j].ing_name);
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
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (size_t j = 0; j < recipes[i].ing_count; j++) {
                        printf("- %s: %s\n", recipes[i].ings[j].ing_name, recipes[i].ings[j].note);
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
            free(recipes[i].ings[j].ing_name);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
