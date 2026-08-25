// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_CLASS 64
#define MAX_STUDENT 64
#define MAX_NOTE 4096

typedef struct Student {
    char name[MAX_STUDENT];
    char note[MAX_NOTE];
    struct Student *next;
} Student;

typedef struct Class {
    char name[MAX_CLASS];
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

void free_students(Student *s) {
    while (s) {
        Student *tmp = s;
        s = s->next;
        free(tmp);
    }
}

void free_all() {
    Class *c = head;
    while (c) {
        Class *tmp = c;
        free_students(c->students);
        c = c->next;
        free(tmp);
    }
    head = NULL;
}

void handle_class(char *line) {
    char name[MAX_CLASS];
    if (sscanf(line, "CLASS %63s", name) != 1) return;
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    strcpy(c->name, name);
    c->students = NULL;
    c->next = head;
    head = c;
}

void handle_student(char *line) {
    char cname[MAX_CLASS], sname[MAX_STUDENT], note[MAX_NOTE];
    int n = sscanf(line, "STUDENT %63s %63s %4095[^\n]", cname, sname, note);
    if (n < 3) return;
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    strcpy(s->name, sname);
    strcpy(s->note, note);
    s->next = c->students;
    c->students = s;
}

void handle_transfer(char *line) {
    char from[MAX_CLASS], to[MAX_CLASS], sname[MAX_STUDENT];
    if (sscanf(line, "TRANSFER %63s %63s %63s", from, to, sname) != 3) return;
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = find_student(cf, sname);
    if (!s) return;
    // remove from cf
    Student *cur = cf->students, *prev = NULL;
    while (cur) {
        if (cur == s) {
            if (prev) prev->next = cur->next;
            else cf->students = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // add to ct
    s->next = ct->students;
    ct->students = s;
}

void handle_drop(char *line) {
    char cname[MAX_CLASS], sname[MAX_STUDENT];
    if (sscanf(line, "DROP %63s %63s", cname, sname) != 2) return;
    Class *c = find_class(cname);
    if (!c) return;
    Student *s = c->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void handle_deleteclass(char *line) {
    char name[MAX_CLASS];
    if (sscanf(line, "DELETECLASS %63s", name) != 1) return;
    Class *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free_students(c->students);
            free(c);
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
            printf("  %s %s\n", s->name, s->note);
            s = s->next;
        }
        c = c->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "CLASS ", 6) == 0) handle_class(line);
        else if (strncmp(line, "STUDENT ", 8) == 0) handle_student(line);
        else if (strncmp(line, "TRANSFER ", 9) == 0) handle_transfer(line);
        else if (strncmp(line, "DROP ", 5) == 0) handle_drop(line);
        else if (strncmp(line, "DELETECLASS ", 12) == 0) handle_deleteclass(line);
        else if (strcmp(line, "PRINT") == 0) handle_print();
    }
    free_all();
    return 0;
}