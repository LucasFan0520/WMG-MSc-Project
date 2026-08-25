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
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *ingredient;
    char *note;
} Ing;

typedef struct {
    char *name;
    Ing *ings;
    int ing_count;
    int ing_cap;
} Recipe;

int main() {
    Recipe *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Recipe *narr = realloc(arr, cap * sizeof(Recipe));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].name = mystrdup(name);
                    arr[count].ings = NULL;
                    arr[count].ing_count = 0;
                    arr[count].ing_cap = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *ingredient = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            size_t nl = strlen(note);
            while (nl > 0 && (note[nl-1] == '\r' || note[nl-1] == '\n')) {
                note[nl-1] = '\0';
                nl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    if (arr[i].ing_count >= arr[i].ing_cap) {
                        arr[i].ing_cap = arr[i].ing_cap == 0 ? 4 : arr[i].ing_cap * 2;
                        Ing *ning = realloc(arr[i].ings, arr[i].ing_cap * sizeof(Ing));
                        if (ning) arr[i].ings = ning;
                    }
                    if (arr[i].ing_count < arr[i].ing_cap) {
                        arr[i].ings[arr[i].ing_count].ingredient = mystrdup(ingredient);
                        arr[i].ings[arr[i].ing_count].note = mystrdup(note);
                        arr[i].ing_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *ingredient = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    for (int j = 0; j < arr[i].ing_count; j++) {
                        if (strcmp(arr[i].ings[j].ingredient, ingredient) == 0) {
                            free(arr[i].ings[j].ingredient);
                            free(arr[i].ings[j].note);
                            for (int k = j; k < arr[i].ing_count - 1; k++) {
                                arr[i].ings[k] = arr[i].ings[k + 1];
                            }
                            arr[i].ing_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (int j = 0; j < arr[i].ing_count; j++) {
                        free(arr[i].ings[j].ingredient);
                        free(arr[i].ings[j].note);
                    }
                    free(arr[i].ings);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("Recipe: %s\n", arr[i].name);
                    for (int j = 0; j < arr[i].ing_count; j++) {
                        printf("  %s: %s\n", arr[i].ings[j].ingredient, arr[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].ing_count; j++) {
            free(arr[i].ings[j].ingredient);
            free(arr[i].ings[j].note);
        }
        free(arr[i].ings);
    }
    free(arr);
    return 0;
}
