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

void free_students(Student *s) {
    while (s) { Student *tmp = s; s = s->next; free(tmp->name); free(tmp->note); free(tmp); }
}

void free_list() {
    while (head) {
        Class *c = head; head = head->next;
        free(c->name); free_students(c->students); free(c);
    }
}

Class *find_class(const char *name) {
    for (Class *c = head; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

Student *find_student(Class *c, const char *name) {
    for (Student *s = c->students; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], c1[256], c2[256], name[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", c1) != 1) continue;
            if (find_class(c1)) continue;
            Class *c = malloc(sizeof(Class));
            c->name = strdup(c1); c->students = NULL; c->next = head; head = c;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", c1, name, note) < 2) continue;
            Class *c = find_class(c1);
            if (c && !find_student(c, name)) {
                Student *s = malloc(sizeof(Student));
                s->name = strdup(name); s->note = strdup(note);
                s->next = c->students; c->students = s;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", c1, c2, name) != 3) continue;
            Class *src = find_class(c1), *dst = find_class(c2);
            if (src && dst) {
                Student *prev = NULL, *curr = src->students;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else src->students = curr->next;
                        curr->next = dst->students; dst->students = curr;
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", c1, name) != 2) continue;
            Class *c = find_class(c1);
            if (c) {
                Student *prev = NULL, *curr = c->students;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        Student *tmp = curr; curr = curr->next;
                        if (prev) prev->next = curr; else c->students = curr;
                        free(tmp->name); free(tmp->note); free(tmp);
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", c1) != 1) continue;
            Class *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, c1) == 0) {
                    Class *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free_students(tmp->students); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Class *c = head; c; c = c->next) {
                printf("%s:\n", c->name);
                for (Student *s = c->students; s; s = s->next)
                    printf("  %s %s\n", s->name, s->note);
            }
        }
    }
    free_list();
    return 0;
}