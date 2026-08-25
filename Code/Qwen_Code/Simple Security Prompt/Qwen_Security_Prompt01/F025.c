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

Class *find_class(const char *name) {
    for (Class *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = head;
    head = c;
}

void add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = cf->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next;
            else cf->students = s->next;
            s->next = ct->students;
            ct->students = s;
            return;
        }
        prev = s; s = s->next;
    }
}

void drop_student(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *s = c->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            free(s->name); free(s->note); free(s);
            return;
        }
        prev = s; s = s->next;
    }
}

void delete_class(const char *name) {
    Class *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            Student *s = p->students;
            while (s) {
                Student *ns = s->next;
                free(s->name); free(s->note); free(s);
                s = ns;
            }
            free(p->name); free(p);
            return;
        }
        prev = p; p = p->next;
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
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000], c[8000];
        c[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "CLASS") == 0) {
            sscanf(line, "%*s %s", a);
            add_class(a);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            sscanf(line, "%*s %s %s %[^\n]", a, b, c);
            add_student(a, b, c);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, c);
            transfer_student(a, b, c);
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            drop_student(a, b);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            sscanf(line, "%*s %s", a);
            delete_class(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    while (head) {
        Class *n = head->next;
        Student *s = head->students;
        while (s) {
            Student *ns = s->next;
            free(s->name); free(s->note); free(s);
            s = ns;
        }
        free(head->name); free(head);
        head = n;
    }
    return 0;
}