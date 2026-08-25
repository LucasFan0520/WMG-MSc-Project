// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int count;
    int cap;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    int r_count = 0;
    int r_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "RECIPE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                if (r_count >= r_cap) {
                    r_cap = r_cap == 0 ? 16 : r_cap * 2;
                    recipes = realloc(recipes, r_cap * sizeof(Recipe));
                }
                recipes[r_count].name = strdup(name);
                recipes[r_count].ings = NULL;
                recipes[r_count].count = 0;
                recipes[r_count].cap = 0;
                r_count++;
            }
        } else if (strncmp(line, "ING ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *ing = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    for (int i = 0; i < r_count; i++) {
                        if (strcmp(recipes[i].name, name) == 0) {
                            Recipe *r = &recipes[i];
                            if (r->count >= r->cap) {
                                r->cap = r->cap == 0 ? 16 : r->cap * 2;
                                r->ings = realloc(r->ings, r->cap * sizeof(Ingredient));
                            }
                            r->ings[r->count].ingredient = strdup(ing);
                            r->ings[r->count].note = strdup(note);
                            r->count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVEING ", 10) == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *ing = p + 1;
                while (*ing == ' ') ing++;
                for (int i = 0; i < r_count; i++) {
                    if (strcmp(recipes[i].name, name) == 0) {
                        Recipe *r = &recipes[i];
                        int idx = -1;
                        for (int j = 0; j < r->count; j++) {
                            if (strcmp(r->ings[j].ingredient, ing) == 0) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx != -1) {
                            free(r->ings[idx].ingredient);
                            free(r->ings[idx].note);
                            for (int j = idx; j < r->count - 1; j++) {
                                r->ings[j] = r->ings[j + 1];
                            }
                            r->count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETERECIPE ", 13) == 0) {
            char *name = line + 13;
            while (*name == ' ') name++;
            int idx = -1;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(recipes[idx].name);
                for (int j = 0; j < recipes[idx].count; j++) {
                    free(recipes[idx].ings[j].ingredient);
                    free(recipes[idx].ings[j].note);
                }
                free(recipes[idx].ings);
                for (int i = idx; i < r_count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                r_count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    printf("Recipe: %s\n", recipes[i].name);
                    for (int j = 0; j < recipes[i].count; j++) {
                        printf("%s - %s\n", recipes[i].ings[j].ingredient, recipes[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < r_count; i++) {
        free(recipes[i].name);
        for (int j = 0; j < recipes[i].count; j++) {
            free(recipes[i].ings[j].ingredient);
            free(recipes[i].ings[j].note);
        }
        free(recipes[i].ings);
    }
    free(recipes);
    return 0;
}
