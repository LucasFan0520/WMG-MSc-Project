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
    Class *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

Student* find_student(Class *c, const char *name) {
    Student *cur = c->students;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    Class *new = malloc(sizeof(Class));
    new->name = strdup(name);
    new->students = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Class *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void add_student(const char *classname, const char *student, const char *note) {
    Class *c = find_class(classname);
    if (!c) return;
    if (find_student(c, student)) return;
    Student *new = malloc(sizeof(Student));
    new->name = strdup(student);
    new->note = strdup(note);
    new->next = NULL;
    if (!c->students) c->students = new;
    else {
        Student *cur = c->students;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void transfer_student(const char *from, const char *to, const char *student) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL, *cur = cf->students;
    while (cur) {
        if (strcmp(cur->name, student) == 0) {
            if (prev) prev->next = cur->next;
            else cf->students = cur->next;
            cur->next = NULL;
            if (!ct->students) ct->students = cur;
            else {
                Student *s = ct->students;
                while (s->next) s = s->next;
                s->next = cur;
            }
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void drop_student(const char *classname, const char *student) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *prev = NULL, *cur = c->students;
    while (cur) {
        if (strcmp(cur->name, student) == 0) {
            if (prev) prev->next = cur->next;
            else c->students = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_class(const char *name) {
    Class *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            Student *s = cur->students;
            while (s) {
                Student *tmp = s;
                s = s->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_classes() {
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

void free_all() {
    Class *cur = head;
    while (cur) {
        Class *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        Student *s = tmp->students;
        while (s) {
            Student *ts = s;
            s = s->next;
            free(ts->name);
            free(ts->note);
            free(ts);
        }
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) add_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *classname = strtok(NULL, " ");
            if (!classname) continue;
            char *student = strtok(NULL, " ");
            if (!student) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_student(classname, student, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from = strtok(NULL, " ");
            if (!from) continue;
            char *to = strtok(NULL, " ");
            if (!to) continue;
            char *student = strtok(NULL, " ");
            if (student) transfer_student(from, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *classname = strtok(NULL, " ");
            if (!classname) continue;
            char *student = strtok(NULL, " ");
            if (student) drop_student(classname, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_all();
    return 0;
}