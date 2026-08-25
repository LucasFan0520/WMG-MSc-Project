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
    Class *curr = classes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

Student *find_student(Class *c, const char *name) {
    Student *curr = c->students;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) exit(EXIT_FAILURE);
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) exit(EXIT_FAILURE);
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *sname) {
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

void drop_student(const char *cname, const char *sname) {
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
    Class *curr = classes;
    Class *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else classes = curr->next;
            Student *s = curr->students;
            while (s) {
                Student *next_s = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next_s;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_classes() {
    Class *c = classes;
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
    Class *curr = classes;
    while (curr) {
        Class *next_c = curr->next;
        Student *s = curr->students;
        while (s) {
            Student *next_s = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = next_s;
        }
        free(curr->name);
        free(curr);
        curr = next_c;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char arg4[3200] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        
        if (strcmp(cmd, "CLASS") == 0) {
            create_class(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && matched >= 4) {
            add_student(arg1, arg2, arg4);
        } else if (strcmp(cmd, "TRANSFER") == 0 && matched >= 3) {
            transfer_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && matched >= 2) {
            drop_student(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_all();
    return 0;
}