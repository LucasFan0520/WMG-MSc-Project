// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

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

Recipe *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Recipe *find_recipe(const char *name) {
    for (Recipe *r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

Ing *find_ing(Recipe *r, const char *name) {
    for (Ing *i = r->ings; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void free_recipe(Recipe *r) {
    Ing *i = r->ings;
    while (i) {
        Ing *next = i->next;
        free(i->name);
        free(i->note);
        free(i);
        i = next;
    }
    free(r->name);
    free(r);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_recipe(name)) {
                Recipe *r = malloc(sizeof(Recipe));
                if (r) {
                    r->name = strdup(name);
                    r->ings = NULL;
                    r->next = head;
                    head = r;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *rname = get_token(&ptr);
            char *iname = get_token(&ptr);
            if (rname && iname) {
                Recipe *r = find_recipe(rname);
                if (r) {
                    Ing *i = find_ing(r, iname);
                    if (!i) {
                        i = malloc(sizeof(Ing));
                        if (i) {
                            i->name = strdup(iname);
                            i->note = strdup(ptr);
                            i->next = r->ings;
                            r->ings = i;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *rname = get_token(&ptr);
            char *iname = get_token(&ptr);
            if (rname && iname) {
                Recipe *r = find_recipe(rname);
                if (r) {
                    Ing *prev = NULL, *curr = r->ings;
                    while (curr) {
                        if (strcmp(curr->name, iname) == 0) {
                            if (prev) prev->next = curr->next;
                            else r->ings = curr->next;
                            free(curr->name);
                            free(curr->note);
                            free(curr);
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Recipe *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free_recipe(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Recipe *r = find_recipe(name);
                if (r) {
                    printf("%s\n", r->name);
                    for (Ing *i = r->ings; i; i = i->next) {
                        printf("%s %s\n", i->name, i->note);
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Recipe *r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    while (head) {
        Recipe *next = head->next;
        free_recipe(head);
        head = next;
    }
    return 0;
}