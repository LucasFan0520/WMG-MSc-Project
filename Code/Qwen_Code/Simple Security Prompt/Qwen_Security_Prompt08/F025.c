// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stud {
    char *name;
    char *note;
    struct Stud *next;
} Stud;

typedef struct Class {
    char *name;
    Stud *students;
    struct Class *next;
} Class;

Class *head = NULL;

Class* find_class(char *name) {
    for (Class *c = head; c; c = c->next) {
        if (!strcmp(c->name, name)) return c;
    }
    return NULL;
}

int main() {
    char cmd[20];
    char cname[100];
    char sname[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "CLASS")) {
            scanf("%s", cname);
            Class *c = malloc(sizeof(Class));
            c->name = strdup(cname);
            c->students = NULL;
            c->next = head;
            head = c;
        } else if (!strcmp(cmd, "STUDENT")) {
            scanf("%s %s", cname, sname);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            Class *c = find_class(cname);
            if (c) {
                Stud *s = malloc(sizeof(Stud));
                s->name = strdup(sname);
                s->note = strdup(note);
                s->next = c->students;
                c->students = s;
            }
        } else if (!strcmp(cmd, "TRANSFER")) {
            char from[100], to[100];
            scanf("%s %s %s", from, to, sname);
            Class *cf = find_class(from);
            Class *ct = find_class(to);
            if (cf && ct) {
                Stud *p = cf->students;
                Stud *prev = NULL;
                while (p) {
                    if (!strcmp(p->name, sname)) {
                        if (prev) prev->next = p->next;
                        else cf->students = p->next;
                        p->next = ct->students;
                        ct->students = p;
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s %s", cname, sname);
            Class *c = find_class(cname);
            if (c) {
                Stud *p = c->students;
                Stud *prev = NULL;
                while (p) {
                    if (!strcmp(p->name, sname)) {
                        if (prev) prev->next = p->next;
                        else c->students = p->next;
                        free(p->name);
                        free(p->note);
                        free(p);
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "DELETECLASS")) {
            scanf("%s", cname);
            Class *p = head;
            Class *prev = NULL;
            while (p) {
                if (!strcmp(p->name, cname)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Stud *s = p->students;
                    while (s) {
                        Stud *tmp = s;
                        s = s->next;
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
        } else if (!strcmp(cmd, "PRINT")) {
            for (Class *c = head; c; c = c->next) {
                printf("%s\n", c->name);
                for (Stud *s = c->students; s; s = s->next) {
                    printf("  %s %s\n", s->name, s->note);
                }
            }
        }
    }
    return 0;
}