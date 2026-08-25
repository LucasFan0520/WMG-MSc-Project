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

Class *classes = NULL;

Class *find_class(const char *name) {
    for (Class *c = classes; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
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

void transfer(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student **sp = &cf->students;
    while (*sp) {
        if (strcmp((*sp)->name, sname) == 0) {
            Student *tmp = *sp;
            *sp = tmp->next;
            tmp->next = ct->students;
            ct->students = tmp;
            return;
        }
        sp = &(*sp)->next;
    }
}

void drop(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student **sp = &c->students;
    while (*sp) {
        if (strcmp((*sp)->name, sname) == 0) {
            Student *tmp = *sp;
            *sp = tmp->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
            return;
        }
        sp = &(*sp)->next;
    }
}

void delete_class(const char *name) {
    Class **cp = &classes;
    while (*cp) {
        if (strcmp((*cp)->name, name) == 0) {
            Class *tmp = *cp;
            *cp = tmp->next;
            Student *s = tmp->students;
            while (s) {
                Student *next_s = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next_s;
            }
            free(tmp->name);
            free(tmp);
            return;
        }
        cp = &(*cp)->next;
    }
}

void print_classes() {
    for (Class *c = classes; c; c = c->next) {
        printf("%s\n", c->name);
        for (Student *s = c->students; s; s = s->next) {
            printf("%s %s\n", s->name, s->note);
        }
    }
}

void cleanup() {
    while (classes) {
        Class *tmp = classes->next;
        Student *s = classes->students;
        while (s) {
            Student *next_s = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = next_s;
        }
        free(classes->name);
        free(classes);
        classes = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) add_class(a1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) add_student(a1, a2, t + 1); } }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) transfer(a1, a2, a3);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) drop(a1, a2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_class(a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    cleanup();
    return 0;
}