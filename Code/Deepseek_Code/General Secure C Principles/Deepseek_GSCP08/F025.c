// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student { char *name; char *note; struct Student *next; } Student;
typedef struct Class { char *name; Student *students; struct Class *next; } Class;
Class *classes = NULL;

Class *find_class(const char *name) {
    for (Class *c = classes; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

Student *find_student(Class *c, const char *name) {
    for (Student *s = c->students; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void student_add(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from), *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL;
    for (Student *s = cf->students; s; prev = s, s = s->next) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next; else cf->students = s->next;
            s->next = ct->students;
            ct->students = s;
            return;
        }
    }
}

void drop_student(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *prev = NULL;
    for (Student *s = c->students; s; prev = s, s = s->next) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next; else c->students = s->next;
            free(s->name); free(s->note); free(s);
            return;
        }
    }
}

void delete_class(const char *name) {
    Class *prev = NULL;
    for (Class *c = classes; c; prev = c, c = c->next) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next; else classes = c->next;
            free(c->name);
            while (c->students) { Student *s = c->students; c->students = s->next; free(s->name); free(s->note); free(s); }
            free(c);
            return;
        }
    }
}

void print_classes(void) {
    for (Class *c = classes; c; c = c->next) {
        printf("%s\n", c->name);
        for (Student *s = c->students; s; s = s->next)
            printf("%s %s\n", s->name, s->note);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) class_create(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cname[256], sname[256], note[4096];
            if (sscanf(rest, "%255s %255s", cname, sname) == 2) {
                char *n = rest + strlen(cname) + 1 + strlen(sname);
                while (*n == ' ') n++;
                strcpy(note, n);
                student_add(cname, sname, note);
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], sname[256];
            if (sscanf(rest, "%255s %255s %255s", from, to, sname) == 3)
                transfer_student(from, to, sname);
        } else if (strcmp(cmd, "DROP") == 0) {
            char cname[256], sname[256];
            if (sscanf(rest, "%255s %255s", cname, sname) == 2)
                drop_student(cname, sname);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    while (classes) { Class *c = classes; classes = c->next; free(c->name); while (c->students) { Student *s = c->students; c->students = s->next; free(s->name); free(s->note); free(s); } free(c); }
    return 0;
}