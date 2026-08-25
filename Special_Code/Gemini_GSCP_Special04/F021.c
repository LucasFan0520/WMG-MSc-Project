/* F021.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char **ing;
    char **note;
    size_t count;
    size_t cap;
} Recipe;

int main(void) {
    Recipe *recipes = NULL;
    size_t r_count = 0;
    size_t r_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
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
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (r_count >= r_cap) {
                    r_cap = r_cap == 0 ? 4 : r_cap * 2;
                    Recipe *nr = realloc(recipes, r_cap * sizeof(Recipe));
                    if (nr) recipes = nr;
                }
                char *nn = mystrdup(name);
                if (nn) {
                    recipes[r_count].name = nn;
                    recipes[r_count].ing = NULL;
                    recipes[r_count].note = NULL;
                    recipes[r_count].count = 0;
                    recipes[r_count].cap = 0;
                    r_count++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *ingredient = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            int found = -1;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Recipe *r = &recipes[found];
                if (r->count >= r->cap) {
                    r->cap = r->cap == 0 ? 4 : r->cap * 2;
                    char **ni = realloc(r->ing, r->cap * sizeof(char *));
                    char **nn = realloc(r->note, r->cap * sizeof(char *));
                    if (ni) r->ing = ni;
                    if (nn) r->note = nn;
                }
                char *ning = mystrdup(ingredient);
                char *nnote = mystrdup(note);
                if (ning && nnote) {
                    r->ing[r->count] = ning;
                    r->note[r->count] = nnote;
                    r->count++;
                } else {
                    free(ning);
                    free(nnote);
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *ingredient = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Recipe *r = &recipes[found];
                int idx = -1;
                for (size_t j = 0; j < r->count; j++) {
                    if (strcmp(r->ing[j], ingredient) == 0) {
                        idx = (int)j;
                        break;
                    }
                }
                if (idx != -1) {
                    free(r->ing[idx]);
                    free(r->note[idx]);
                    for (size_t j = (size_t)idx; j < r->count - 1; j++) {
                        r->ing[j] = r->ing[j + 1];
                        r->note[j] = r->note[j + 1];
                    }
                    r->count--;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(recipes[found].name);
                for (size_t j = 0; j < recipes[found].count; j++) {
                    free(recipes[found].ing[j]);
                    free(recipes[found].note[j]);
                }
                free(recipes[found].ing);
                free(recipes[found].note);
                for (size_t i = (size_t)found; i < r_count - 1; i++) {
                    recipes[i] = recipes[i + 1];
                }
                r_count--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(recipes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", recipes[found].name);
                for (size_t j = 0; j < recipes[found].count; j++) {
                    printf("%s %s\n", recipes[found].ing[j], recipes[found].note[j]);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s\n", recipes[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < r_count; i++) {
        free(recipes[i].name);
        for (size_t j = 0; j < recipes[i].count; j++) {
            free(recipes[i].ing[j]);
            free(recipes[i].note[j]);
        }
        free(recipes[i].ing);
        free(recipes[i].note);
    }
    free(recipes);
    return 0;
}
