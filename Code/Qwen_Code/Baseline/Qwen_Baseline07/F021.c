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

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

Rec *find_rec(const char *name) {
    for (Rec *r = head; r; r = r->next) if (strcmp(r->name, name) == 0) return r;
    return NULL;
}

int main() {
    char cmd[20], name[256], ing_name[256], note[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "RECIPE") == 0) {
            scanf("%s", name);
            Rec *r = malloc(sizeof(Rec));
            r->name = strdup(name); r->ings = NULL; r->next = head; head = r;
        } else if (strcmp(cmd, "ING") == 0) {
            scanf("%s %s", name, ing_name);
            read_rest(note, sizeof(note));
            Rec *r = find_rec(name);
            if (r) {
                Ing *i = malloc(sizeof(Ing));
                i->name = strdup(ing_name); i->note = strdup(note); i->next = r->ings; r->ings = i;
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            scanf("%s %s", name, ing_name);
            Rec *r = find_rec(name);
            if (r) {
                Ing *prev = NULL, *curr = r->ings;
                while (curr) {
                    if (strcmp(curr->name, ing_name) == 0) {
                        if (prev) prev->next = curr->next; else r->ings = curr->next;
                        free(curr->name); free(curr->note); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            scanf("%s", name);
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->ings) { Ing *ni = curr->ings->next; free(curr->ings->name); free(curr->ings->note); free(curr->ings); curr->ings = ni; }
                    free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            Rec *r = find_rec(name);
            if (r) {
                printf("%s\n", r->name);
                for (Ing *i = r->ings; i; i = i->next) printf("%s %s\n", i->name, i->note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Rec *r = head; r; r = r->next) printf("%s\n", r->name);
        }
    }
    return 0;
}