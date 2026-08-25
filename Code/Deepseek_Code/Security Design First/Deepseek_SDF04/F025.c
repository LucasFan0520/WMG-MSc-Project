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
    Class *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

Student *find_student(Class *c, const char *name) {
    Student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void create_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = strdup(name);
    c->students = NULL;
    c->next = NULL;
    if (!c->name) { free(c); return; }
    if (!head) head = c;
    else {
        Class *last = head;
        while (last->next) last = last->next;
        last->next = c;
    }
}

void add_student(const char *classname, const char *student, const char *note) {
    Class *c = find_class(classname);
    if (!c) return;
    if (find_student(c, student)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(student);
    s->note = strdup(note);
    s->next = NULL;
    if (!s->name || !s->note) {
        free(s->name); free(s->note); free(s);
        return;
    }
    if (!c->students) c->students = s;
    else {
        Student *last = c->students;
        while (last->next) last = last->next;
        last->next = s;
    }
}

void transfer_student(const char *from, const char *to, const char *student) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = cf->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, student) == 0) {
            if (prev) prev->next = s->next;
            else cf->students = s->next;
            s->next = NULL;
            if (!ct->students) ct->students = s;
            else {
                Student *last = ct->students;
                while (last->next) last = last->next;
                last->next = s;
            }
            return;
        }
        prev = s;
        s = s->next;
    }
}

void drop_student(const char *classname, const char *student) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *s = c->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, student) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void delete_class(const char *name) {
    Class *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c->name);
            Student *s = c->students;
            while (s) {
                Student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void print_classes(void) {
    Class *c = head;
    while (c) {
        printf("%s\n", c->name);
        Student *s = c->students;
        while (s) {
            printf("%s %s\n", s->name, s->note);
            s = s->next;
        }
        c = c->next;
    }
}

void free_all(void) {
    Class *c = head;
    while (c) {
        Class *next = c->next;
        free(c->name);
        Student *s = c->students;
        while (s) {
            Student *snext = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = snext;
        }
        free(c);
        c = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], c1[256], c2[256], student[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", c1) == 1)
                create_class(c1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", c1, student, note) == 3)
                add_student(c1, student, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", c1, c2, student) == 3)
                transfer_student(c1, c2, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", c1, student) == 2)
                drop_student(c1, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", c1) == 1)
                delete_class(c1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free(line);
    free_all();
    return 0;
}