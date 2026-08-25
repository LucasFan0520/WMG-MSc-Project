// F025.c
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
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Stud {
    char* name;
    char* note;
    struct Stud* next;
} Stud;

typedef struct Class {
    char* name;
    Stud* studs;
    struct Class* next;
} Class;

Class* head = NULL;

Class* find_class(const char* name) {
    for (Class* c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

Stud* find_stud(Class* c, const char* name) {
    for (Stud* s = c->studs; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
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

        if (strcmp(cmd, "CLASS") == 0) {
            char* name = get_word(&p);
            if (name && !find_class(name)) {
                Class* n = malloc(sizeof(Class));
                n->name = strdup(name);
                n->studs = NULL;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char* cname = get_word(&p);
            char* sname = get_word(&p);
            Class* c = find_class(cname);
            if (c && sname) {
                Stud* n = malloc(sizeof(Stud));
                n->name = strdup(sname);
                n->note = strdup(p);
                n->next = c->studs;
                c->studs = n;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char* from = get_word(&p);
            char* to = get_word(&p);
            char* sname = get_word(&p);
            Class* cf = find_class(from);
            Class* ct = find_class(to);
            if (cf && ct && sname) {
                Stud* prev = NULL;
                for (Stud* s = cf->studs; s; prev = s, s = s->next) {
                    if (strcmp(s->name, sname) == 0) {
                        if (prev) prev->next = s->next;
                        else cf->studs = s->next;
                        s->next = ct->studs;
                        ct->studs = s;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char* cname = get_word(&p);
            char* sname = get_word(&p);
            Class* c = find_class(cname);
            if (c) {
                Stud* prev = NULL;
                for (Stud* s = c->studs; s; prev = s, s = s->next) {
                    if (strcmp(s->name, sname) == 0) {
                        if (prev) prev->next = s->next;
                        else c->studs = s->next;
                        free(s->name);
                        free(s->note);
                        free(s);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char* name = get_word(&p);
            Class* prev = NULL;
            for (Class* c = head; c; prev = c, c = c->next) {
                if (strcmp(c->name, name) == 0) {
                    if (prev) prev->next = c->next;
                    else head = c->next;
                    free(c->name);
                    while (c->studs) {
                        Stud* temp = c->studs;
                        c->studs = c->studs->next;
                        free(temp->name);
                        free(temp->note);
                        free(temp);
                    }
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Class* c = head; c; c = c->next) {
                printf("%s\n", c->name);
                for (Stud* s = c->studs; s; s = s->next) {
                    printf("%s %s\n", s->name, s->note);
                }
            }
        }
    }
    return 0;
}