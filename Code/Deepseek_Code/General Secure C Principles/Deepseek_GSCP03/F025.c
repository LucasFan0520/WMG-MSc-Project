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

void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = strdup(name);
    c->students = NULL;
    c->next = head;
    head = c;
}

void student_add(const char *class_name, const char *student, const char *note) {
    Class *c = find_class(class_name);
    if (!c) return;
    if (find_student(c, student)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(student);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void student_transfer(const char *from, const char *to, const char *student) {
    Class *fc = find_class(from);
    Class *tc = find_class(to);
    if (!fc || !tc) return;
    Student *s = find_student(fc, student);
    if (!s) return;
    if (find_student(tc, student)) return;
    // remove from fc
    Student *prev = NULL, *curr = fc->students;
    while (curr) {
        if (curr == s) {
            if (prev) prev->next = curr->next;
            else fc->students = curr->next;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    // add to tc
    s->next = tc->students;
    tc->students = s;
}

void student_drop(const char *class_name, const char *student) {
    Class *c = find_class(class_name);
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

void class_delete(const char *name) {
    Class *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Student *s = curr->students;
            while (s) {
                Student *tmp = s;
                s = s->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void class_print(void) {
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
        Class *tmp = c;
        c = c->next;
        free(tmp->name);
        Student *s = tmp->students;
        while (s) {
            Student *tmp_s = s;
            s = s->next;
            free(tmp_s->name);
            free(tmp_s->note);
            free(tmp_s);
        }
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            class_create(p);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256], stu[256];
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", cls, stu) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    student_add(cls, stu, note);
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], stu[256];
            if (sscanf(line + 9, "%255s %255s %255s", from, to, stu) == 3) {
                student_transfer(from, to, stu);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256], stu[256];
            if (sscanf(line + 5, "%255s %255s", cls, stu) == 2) {
                student_drop(cls, stu);
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *p = line + 12;
            while (*p == ' ') p++;
            class_delete(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            class_print();
        }
    }
    free_all();
    return 0;
}