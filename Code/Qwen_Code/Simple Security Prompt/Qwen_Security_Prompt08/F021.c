// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ing {
    char *name;
    char *note;
    struct Ing *next;
} Ing;

typedef struct Rec {
    char *name;
    Ing *ings;
    struct Rec *next;
} Rec;

Rec *head = NULL;

Rec* find_rec(char *name) {
    for (Rec *r = head; r; r = r->next) {
        if (!strcmp(r->name, name)) return r;
    }
    return NULL;
}

int main() {
    char cmd[20];
    char name[100];
    char ing[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "RECIPE")) {
            scanf("%s", name);
            Rec *r = malloc(sizeof(Rec));
            r->name = strdup(name);
            r->ings = NULL;
            r->next = head;
            head = r;
        } else if (!strcmp(cmd, "ING")) {
            scanf("%s %s", name, ing);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            Rec *r = find_rec(name);
            if (r) {
                Ing *i = malloc(sizeof(Ing));
                i->name = strdup(ing);
                i->note = strdup(note);
                i->next = r->ings;
                r->ings = i;
            }
        } else if (!strcmp(cmd, "REMOVEING")) {
            scanf("%s %s", name, ing);
            Rec *r = find_rec(name);
            if (r) {
                Ing *p = r->ings;
                Ing *prev = NULL;
                while (p) {
                    if (!strcmp(p->name, ing)) {
                        if (prev) prev->next = p->next;
                        else r->ings = p->next;
                        free(p->name);
                        free(p->note);
                        free(p);
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "DELETERECIPE")) {
            scanf("%s", name);
            Rec *p = head;
            Rec *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Ing *i = p->ings;
                    while (i) {
                        Ing *tmp = i;
                        i = i->next;
                        free(tmp->name);
                        free(tmp->note);
                        free(tmp);
                    }
                    free(p->name);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            Rec *r = find_rec(name);
            if (r) {
                printf("%s\n", r->name);
                for (Ing *i = r->ings; i; i = i->next) {
                    printf("%s %s\n", i->name, i->note);
                }
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s\n", r->name);
            }
        }
    }
    return 0;
}