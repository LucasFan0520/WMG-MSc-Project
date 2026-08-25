// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Ing {
    char *name;
    char *note;
    struct Ing *next;
} Ing;

typedef struct Recipe {
    char *name;
    Ing *ings;
    struct Recipe *next;
} Recipe;

Recipe *recipes = NULL;

Recipe *find_recipe(const char *name) {
    for (Recipe *r = recipes; r; r = r->next) {
        if (strcmp(r->name, name) == 0) return r;
    }
    return NULL;
}

void add_recipe(const char *name) {
    if (find_recipe(name)) return;
    Recipe *r = malloc(sizeof(Recipe));
    r->name = strdup(name);
    r->ings = NULL;
    r->next = recipes;
    recipes = r;
}

void add_ing(const char *rname, const char *iname, const char *note) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ing *i = malloc(sizeof(Ing));
    i->name = strdup(iname);
    i->note = strdup(note);
    i->next = r->ings;
    r->ings = i;
}

void remove_ing(const char *rname, const char *iname) {
    Recipe *r = find_recipe(rname);
    if (!r) return;
    Ing *prev = NULL, *curr = r->ings;
    while (curr) {
        if (strcmp(curr->name, iname) == 0) {
            if (prev) prev->next = curr->next;
            else r->ings = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_recipe(const char *name) {
    Recipe *prev = NULL, *curr = recipes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else recipes = curr->next;
            Ing *i = curr->ings;
            while (i) {
                Ing *next = i->next;
                free(i->name);
                free(i->note);
                free(i);
                i = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_recipe(const char *name) {
    Recipe *r = find_recipe(name);
    if (!r) return;
    printf("%s:\n", r->name);
    for (Ing *i = r->ings; i; i = i->next) {
        printf("%s %s\n", i->name, i->note);
    }
}

void list_recipes() {
    for (Recipe *r = recipes; r; r = r->next) {
        printf("%s\n", r->name);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "RECIPE") == 0) add_recipe(arg1);
        else if (strcmp(cmd, "ING") == 0) {
            char *note = get_rest(line, 3);
            if (note) add_ing(arg1, arg2, note);
        }
        else if (strcmp(cmd, "REMOVEING") == 0) remove_ing(arg1, arg2);
        else if (strcmp(cmd, "DELETERECIPE") == 0) delete_recipe(arg1);
        else if (strcmp(cmd, "SHOW") == 0) show_recipe(arg1);
        else if (strcmp(cmd, "LIST") == 0) list_recipes();
        
        free(line);
    }
    return 0;
}