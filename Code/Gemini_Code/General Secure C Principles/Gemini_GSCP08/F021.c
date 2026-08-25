// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *ingredient;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    size_t ing_count;
    size_t ing_cap;
} Recipe;

int main(void) {
    Recipe *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = args;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    Recipe *narr = realloc(arr, ncap * sizeof(Recipe));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].name = strdup(name);
                    arr[count].ings = NULL;
                    arr[count].ing_count = 0;
                    arr[count].ing_cap = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *ing = p2 + 1;
                char *p3 = strchr(ing, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            Recipe *r = &arr[i];
                            if (r->ing_count >= r->ing_cap) {
                                size_t ncap = r->ing_cap == 0 ? 4 : r->ing_cap * 2;
                                Ingredient *nings = realloc(r->ings, ncap * sizeof(Ingredient));
                                if (nings) {
                                    r->ings = nnings;
                                    r->ing_cap = ncap;
                                }
                            }
                            if (r->ing_count < r->ing_cap) {
                                r->ings[r->ing_count].ingredient = strdup(ing);
                                r->ings[r->ing_count].note = strdup(note);
                                r->ing_count++;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *ing = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        Recipe *r = &arr[i];
                        for (size_t j = 0; j < r->ing_count; j++) {
                            if (strcmp(r->ings[j].ingredient, ing) == 0) {
                                free(r->ings[j].ingredient);
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
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].ing_count; j++) {
                        free(arr[i].ings[j].ingredient);
                        free(arr[i].ings[j].note);
                    }
                    free(arr[i].ings);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("RECIPE %s\n", arr[i].name);
                    for (size_t j = 0; j < arr[i].ing_count; j++) {
                        printf("ING %s %s\n", arr[i].ings[j].ingredient, arr[i].ings[j].note);
                    }
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].ing_count; j++) {
            free(arr[i].ings[j].ingredient);
            free(arr[i].ings[j].note);
        }
        free(arr[i].ings);
    }
    free(arr);
    return 0;
}
