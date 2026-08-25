// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    char *note;
    struct Student *next;
} Student;

typedef struct Class {
    char *name;
    Student *students;
    struct Class *next;
} Class;

Class *head = NULL;

Class *find_class(char *name) {
    for (Class *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

Student *find_student(Class *c, char *name) {
    for (Student *s = c->students; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

void create_class(char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = head;
    head = c;
}

void add_student(char *cname, char *sname, char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(char *from, char *to, char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL;
    Student *curr = cf->students;
    while (curr) {
        if (strcmp(curr->name, sname) == 0) {
            if (prev) prev->next = curr->next;
            else cf->students = curr->next;
            curr->next = ct->students;
            ct->students = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void drop_student(char *cname, char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *prev = NULL;
    Student *curr = c->students;
    while (curr) {
        if (strcmp(curr->name, sname) == 0) {
            if (prev) prev->next = curr->next;
            else c->students = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_class(char *name) {
    Class *prev = NULL;
    Class *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Student *s = curr->students;
            while (s) {
                Student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_classes() {
    for (Class *c = head; c; c = c->next) {
        printf("%s\n", c->name);
        for (Student *s = c->students; s; s = s->next) {
            printf("  %s %s\n", s->name, s->note);
        }
    }
}

int main() {
    char cmd[20], a[100], b[100], c[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CLASS") == 0) {
            scanf("%99s", a);
            create_class(a);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            scanf("%99s%99s", a, b);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_student(a, b, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            scanf("%99s%99s%99s", a, b, c);
            transfer_student(a, b, c);
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%99s%99s", a, b);
            drop_student(a, b);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            scanf("%99s", a);
            delete_class(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    return 0;
}