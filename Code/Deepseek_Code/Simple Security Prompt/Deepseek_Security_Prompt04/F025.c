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

Class *head = NULL;

Class *find_class(char *name) {
    for (Class *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

Student *find_student_in_class(Class *c, char *name) {
    for (Student *s = c->students; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
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

void add_student(char *class, char *student, char *note) {
    Class *c = find_class(class);
    if (!c) return;
    if (find_student_in_class(c, student)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(student);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(char *from, char *to, char *student) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL;
    Student *s = cf->students;
    while (s) {
        if (strcmp(s->name, student) == 0) break;
        prev = s;
        s = s->next;
    }
    if (!s) return;
    if (prev) prev->next = s->next;
    else cf->students = s->next;
    s->next = ct->students;
    ct->students = s;
}

void drop_student(char *class, char *student) {
    Class *c = find_class(class);
    if (!c) return;
    Student *prev = NULL;
    for (Student *s = c->students; s; s = s->next) {
        if (strcmp(s->name, student) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
    }
}

void delete_class(char *name) {
    Class *prev = NULL;
    for (Class *c = head; c; c = c->next) {
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
    }
}

void print_all(void) {
    for (Class *c = head; c; c = c->next) {
        printf("%s\n", c->name);
        for (Student *s = c->students; s; s = s->next)
            printf("  %s %s\n", s->name, s->note);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "CLASS") == 0 && n == 2) {
            create_class(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && n == 4) {
            add_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n == 4) {
            transfer_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n == 3) {
            drop_student(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n == 2) {
            delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_all();
        }
    }
    free(line);
    free_all();
    return 0;
}