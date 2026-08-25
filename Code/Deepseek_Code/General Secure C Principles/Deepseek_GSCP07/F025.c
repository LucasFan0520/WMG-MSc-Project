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

Class* find_class(const char *name) {
    Class *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

Student* find_student_in_class(Class *c, const char *name) {
    Student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void free_students(Student *s) {
    while (s) {
        Student *tmp = s;
        s = s->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

void free_class(Class *c) {
    free(c->name);
    free_students(c->students);
    free(c);
}

void handle_class(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    if (find_class(name)) return;
    Class *new = malloc(sizeof(Class));
    new->name = strdup(name);
    new->students = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Class *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_student(char *cmd) {
    char *cname = strtok(NULL, " ");
    if (!cname) return;
    Class *c = find_class(cname);
    if (!c) return;
    char *sname = strtok(NULL, " ");
    if (!sname) return;
    char *note = strtok(NULL, "");
    if (!note) note = "";
    if (find_student_in_class(c, sname)) return;
    Student *new = malloc(sizeof(Student));
    new->name = strdup(sname);
    new->note = strdup(note);
    new->next = NULL;
    if (!c->students) c->students = new;
    else {
        Student *last = c->students;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_transfer(char *cmd) {
    char *from = strtok(NULL, " ");
    char *to = strtok(NULL, " ");
    char *sname = strtok(NULL, " ");
    if (!from || !to || !sname) return;
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = cf->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
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

void handle_drop(char *cmd) {
    char *cname = strtok(NULL, " ");
    if (!cname) return;
    Class *c = find_class(cname);
    if (!c) return;
    char *sname = strtok(NULL, " ");
    if (!sname) return;
    Student *s = c->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
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

void handle_deleteclass(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Class *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free_class(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void handle_print() {
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "CLASS") == 0) handle_class(cmd);
        else if (strcmp(cmd, "STUDENT") == 0) handle_student(cmd);
        else if (strcmp(cmd, "TRANSFER") == 0) handle_transfer(cmd);
        else if (strcmp(cmd, "DROP") == 0) handle_drop(cmd);
        else if (strcmp(cmd, "DELETECLASS") == 0) handle_deleteclass(cmd);
        else if (strcmp(cmd, "PRINT") == 0) handle_print();
    }
    while (head) {
        Class *tmp = head;
        head = head->next;
        free_class(tmp);
    }
    return 0;
}