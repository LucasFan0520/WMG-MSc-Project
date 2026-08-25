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

Class *head = NULL, *tail = NULL;

Class *find_class(const char *name) {
    Class *c = head;
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

void create_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);
    c->students = NULL;
    c->next = NULL;
    if (tail) {
        tail->next = c;
        tail = c;
    } else {
        head = tail = c;
    }
}

void add_student(const char *classname, const char *student, const char *note) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *s = malloc(sizeof(Student));
    s->name = malloc(strlen(student) + 1);
    strcpy(s->name, student);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *student) {
    Class *fc = find_class(from);
    Class *tc = find_class(to);
    if (!fc || !tc) return;
    Student *prev = NULL, *curr = fc->students;
    while (curr) {
        if (strcmp(curr->name, student) == 0) {
            if (prev) prev->next = curr->next;
            else fc->students = curr->next;
            curr->next = tc->students;
            tc->students = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void drop_student(const char *classname, const char *student) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *prev = NULL, *curr = c->students;
    while (curr) {
        if (strcmp(curr->name, student) == 0) {
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
    Class *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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
        Student *s = c->students;
        while (s) {
            Student *nexts = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = nexts;
        }
        free(c->name);
        free(c);
        c = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) create_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *classname = strtok(NULL, " ");
            char *student = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (classname && student && note) add_student(classname, student, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from = strtok(NULL, " ");
            char *to = strtok(NULL, " ");
            char *student = strtok(NULL, " ");
            if (from && to && student) transfer_student(from, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *classname = strtok(NULL, " ");
            char *student = strtok(NULL, " ");
            if (classname && student) drop_student(classname, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free(line);
    free_all();
    return 0;
}