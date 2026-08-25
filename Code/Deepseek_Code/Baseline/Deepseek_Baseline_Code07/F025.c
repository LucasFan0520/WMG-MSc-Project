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
Class *tail = NULL;

Class *find_class(const char *name) {
    Class *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void create_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = malloc(strlen(name)+1);
    strcpy(c->name, name);
    c->students = NULL;
    c->next = NULL;
    if (tail) tail->next = c;
    else head = c;
    tail = c;
}

void add_student(const char *class_name, const char *student, const char *note) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *s = malloc(sizeof(Student));
    s->name = malloc(strlen(student)+1);
    strcpy(s->name, student);
    s->note = malloc(strlen(note)+1);
    strcpy(s->note, note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *student) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL;
    Student *s = cf->students;
    while (s) {
        if (strcmp(s->name, student) == 0) {
            if (prev) prev->next = s->next;
            else cf->students = s->next;
            s->next = ct->students;
            ct->students = s;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void drop_student(const char *class_name, const char *student) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *prev = NULL;
    Student *s = c->students;
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
    Class *prev = NULL;
    Class *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            if (c == tail) tail = prev;
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = strtok(rest, " ");
            if (name) create_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *class_name = strtok(rest, " ");
            if (!class_name) continue;
            char *student = strtok(NULL, " ");
            if (!student) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_student(class_name, student, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from = strtok(rest, " ");
            char *to = strtok(NULL, " ");
            char *student = strtok(NULL, " ");
            if (from && to && student) transfer_student(from, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *class_name = strtok(rest, " ");
            if (!class_name) continue;
            char *student = strtok(NULL, " ");
            if (student) drop_student(class_name, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_all();
    return 0;
}