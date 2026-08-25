// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Ing {
    char* name;
    char* note;
    struct Ing* next;
} Ing;

typedef struct Recipe {
    char* name;
    Ing* ings;
    struct Recipe* next;
} Recipe;

Recipe* head = NULL;

Recipe* find_recipe(const char* name) {
    for (Recipe* r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "RECIPE") == 0) {
            char* name = get_word(&p);
            if (name && !find_recipe(name)) {
                Recipe* n = malloc(sizeof(Recipe));
                n->name = strdup(name);
                n->ings = NULL;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char* rname = get_word(&p);
            char* iname = get_word(&p);
            Recipe* r = find_recipe(rname);
            if (r && iname) {
                Ing* n = malloc(sizeof(Ing));
                n->name = strdup(iname);
                n->note = strdup(p);
                n->next = r->ings;
                r->ings = n;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char* rname = get_word(&p);
            char* iname = get_word(&p);
            Recipe* r = find_recipe(rname);
            if (r) {
                Ing* prev = NULL;
                for (Ing* i = r->ings; i; prev = i, i = i->next) {
                    if (strcmp(i->name, iname) == 0) {
                        if (prev) prev->next = i->next;
                        else r->ings = i->next;
                        free(i->name);
                        free(i->note);
                        free(i);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char* name = get_word(&p);
            Recipe* prev = NULL;
            for (Recipe* r = head; r; prev = r, r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    if (prev) prev->next = r->next;
                    else head = r->next;
                    free(r->name);
                    while (r->ings) {
                        Ing* temp = r->ings;
                        r->ings = r->ings->next;
                        free(temp->name);
                        free(temp->note);
                        free(temp);
                    }
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char* name = get_word(&p);
            Recipe* r = find_recipe(name);
            if (r) {
                printf("%s\n", r->name);
                for (Ing* i = r->ings; i; i = i->next) {
                    printf("%s %s\n", i->name, i->note);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Recipe* r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    return 0;
}