/* F025.c */
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
    Class *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

Student *find_student_in_class(Class *c, const char *name) {
    Student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
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

void add_student(const char *classname, const char *student, const char *note) {
    Class *c = find_class(classname);
    if (!c) return;
    if (find_student_in_class(c, student)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(student);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *student) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = cf->students, *prev = NULL;
    while (s && strcmp(s->name, student) != 0) {
        prev = s;
        s = s->next;
    }
    if (!s) return;
    if (prev) prev->next = s->next;
    else cf->students = s->next;
    s->next = ct->students;
    ct->students = s;
}

void drop_student(const char *classname, const char *student) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *s = c->students, *prev = NULL;
    while (s && strcmp(s->name, student) != 0) {
        prev = s;
        s = s->next;
    }
    if (!s) return;
    if (prev) prev->next = s->next;
    else c->students = s->next;
    free(s->name);
    free(s->note);
    free(s);
}

void delete_class(const char *name) {
    Class *c = classes, *prev = NULL;
    while (c && strcmp(c->name, name) != 0) {
        prev = c;
        c = c->next;
    }
    if (!c) return;
    if (prev) prev->next = c->next;
    else classes = c->next;
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
}

void print_all(void) {
    Class *c = classes;
    while (c) {
        printf("%s\n", c->name);
        Student *s = c->students;
        while (s) {
            printf("%s ", s->name);
            for (char *p = s->note; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
            s = s->next;
        }
        c = c->next;
    }
}

void free_all(void) {
    Class *c = classes;
    while (c) {
        Class *nextc = c->next;
        free(c->name);
        Student *s = c->students;
        while (s) {
            Student *nexts = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = nexts;
        }
        free(c);
        c = nextc;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            add_class(p);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char classname[256], student[256];
            char *p = line + 8;
            if (sscanf(p, "%255s %255s", classname, student) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_student(classname, student, p);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], student[256];
            if (sscanf(line + 9, "%255s %255s %255s", from, to, student) == 3)
                transfer_student(from, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            char classname[256], student[256];
            if (sscanf(line + 5, "%255s %255s", classname, student) == 2)
                drop_student(classname, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            delete_class(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    free(line);
    return 0;
}