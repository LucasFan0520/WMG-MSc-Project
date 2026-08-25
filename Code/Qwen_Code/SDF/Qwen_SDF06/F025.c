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
    for (Class *c = head; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = strdup(name);
    if (!c->name) { free(c); return; }
    c->students = NULL;
    c->next = head;
    head = c;
}

void add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(sname);
    s->note = strdup(note);
    if (!s->name || !s->note) { free(s->name); free(s->note); free(s); return; }
    s->next = c->students;
    c->students = s;
}

void transfer(const char *from, const char *to, const char *sname) {
    Class *c_from = find_class(from);
    Class *c_to = find_class(to);
    if (!c_from || !c_to) return;
    Student *curr = c_from->students;
    Student *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, sname) == 0) {
            if (prev) prev->next = curr->next;
            else c_from->students = curr->next;
            curr->next = c_to->students;
            c_to->students = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void drop(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *curr = c->students;
    Student *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, sname) == 0) {
            if (prev) prev->next = curr->next;
            else c->students = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_class(const char *name) {
    Class *curr = head;
    Class *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Student *s = curr->students;
            while (s) {
                Student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_all() {
    for (Class *c = head; c; c = c->next) {
        printf("%s\n", c->name);
        for (Student *s = c->students; s; s = s->next) {
            printf("%s %s\n", s->name, s->note);
        }
    }
}

void free_all() {
    Class *c = head;
    while (c) {
        Class *next_c = c->next;
        Student *s = c->students;
        while (s) {
            Student *next_s = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = next_s;
        }
        free(c->name);
        free(c);
        c = next_c;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %1023s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        
        if (strcmp(cmd, "CLASS") == 0 && n >= 2) {
            add_class(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 4) {
            add_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            transfer(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            drop(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n >= 2) {
            delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}