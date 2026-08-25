// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    int icount;
    int icap;
} Recipe;

int main() {
    Recipe *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = get_token(&p);
            if (name) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Recipe *narr = realloc(arr, cap * sizeof(Recipe));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].ings = NULL;
                arr[count].icount = 0;
                arr[count].icap = 0;
                count++;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = get_token(&p);
            char *ing = get_token(&p);
            char *note = get_rest(&p);
            if (name && ing) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        Recipe *r = &arr[i];
                        if (r->icount >= r->icap) {
                            r->icap = r->icap == 0 ? 4 : r->icap * 2;
                            Ingredient *nings = realloc(r->ings, r->icap * sizeof(Ingredient));
                            if (nings) r->ings = nings;
                        }
                        r->ings[r->icount].ing_name = strdup(ing);
                        r->ings[r->icount].note = strdup(note);
                        r->icount++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = get_token(&p);
            char *ing = get_token(&p);
            if (name && ing) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        for (int j = 0; j < arr[i].icount; j++) {
                            if (strcmp(arr[i].ings[j].ing_name, ing) == 0) {
                                free(arr[i].ings[j].ing_name);
                                free(arr[i].ings[j].note);
                                for (int k = j; k < arr[i].icount - 1; k++) {
                                    arr[i].ings[k] = arr[i].ings[k + 1];
                                }
                                arr[i].icount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        free(arr[i].name);
                        for (int j = 0; j < arr[i].icount; j++) {
                            free(arr[i].ings[j].ing_name);
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
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = get_token(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        printf("%s\n", arr[i].name);
                        for (int j = 0; j < arr[i].icount; j++) {
                            printf("%s %s\n", arr[i].ings[j].ing_name, arr[i].ings[j].note);
                        }
                        break;
                    }
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
        for (int j = 0; j < arr[i].icount; j++) {
            free(arr[i].ings[j].ing_name);
            free(arr[i].ings[j].note);
        }
        free(arr[i].ings);
    }
    free(arr);
    return 0;
}
