// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *ing_name;
    char *note;
} Ingredient;

typedef struct {
    char *name;
    Ingredient *ings;
    size_t icount;
    size_t icap;
} Recipe;

int main(void) {
    Recipe *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Recipe *nitems = realloc(items, cap * sizeof(Recipe));
                if (nitems) items = nitems;
            }
            items[count].name = mystrdup(name);
            items[count].ings = NULL;
            items[count].icount = 0;
            items[count].icap = 0;
            count++;
        } else if (strcmp(cmd, "ING") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *ing = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Recipe *r = &items[i];
                    if (r->icount >= r->icap) {
                        r->icap = r->icap == 0 ? 4 : r->icap * 2;
                        Ingredient *ni = realloc(r->ings, r->icap * sizeof(Ingredient));
                        if (ni) r->ings = ni;
                    }
                    r->ings[r->icount].ing_name = mystrdup(ing);
                    r->ings[r->icount].note = mystrdup(note);
                    r->icount++;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *ing = p;
            size_t ilen = strlen(ing);
            while (ilen > 0 && (ing[ilen-1] == '\n' || ing[ilen-1] == '\n')) {
                ing[ilen-1] = '\0';
                ilen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Recipe *r = &items[i];
                    for (size_t j = 0; j < r->icount; j++) {
                        if (strcmp(r->ings[j].ing_name, ing) == 0) {
                            free(r->ings[j].ing_name);
                            free(r->ings[j].note);
                            for (size_t k = j; k < r->icount - 1; k++) {
                                r->ings[k] = r->ings[k+1];
                            }
                            r->icount--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (size_t j = 0; j < items[i].icount; j++) {
                        free(items[i].ings[j].ing_name);
                        free(items[i].ings[j].note);
                    }
                    free(items[i].ings);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("Recipe: %s\n", items[i].name);
                    for (size_t j = 0; j < items[i].icount; j++) {
                        printf("  %s - %s\n", items[i].ings[j].ing_name, items[i].ings[j].note);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (size_t j = 0; j < items[i].icount; j++) {
            free(items[i].ings[j].ing_name);
            free(items[i].ings[j].note);
        }
        free(items[i].ings);
    }
    free(items);
    return 0;
}
