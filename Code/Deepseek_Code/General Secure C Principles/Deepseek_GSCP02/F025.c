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
    Student *stu_head;
    struct Class *next;
} Class;

Class *c_head = NULL;

Class *find_class(const char *name) {
    Class *c = c_head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

Student *find_student_in_class(Class *c, const char *name) {
    Student *s = c->stu_head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void create_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = strdup(name);
    c->stu_head = NULL;
    c->next = c_head;
    c_head = c;
}

void add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student_in_class(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->stu_head;
    c->stu_head = s;
}

void transfer_student(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    if (find_student_in_class(ct, sname)) return;
    Student *prev = NULL;
    Student *s = cf->stu_head;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next;
            else cf->stu_head = s->next;
            s->next = ct->stu_head;
            ct->stu_head = s;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void drop_student(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *prev = NULL;
    Student *s = c->stu_head;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
            if (prev) prev->next = s->next;
            else c->stu_head = s->next;
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
    Class *c = c_head;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else c_head = c->next;
            free(c->name);
            Student *s = c->stu_head;
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

void print_all(void) {
    Class *c = c_head;
    while (c) {
        printf("%s\n", c->name);
        Student *s = c->stu_head;
        while (s) {
            printf("%s ", s->name);
            for (char *p = s->note; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
            s = s->next;
        }
        c = c->next;
    }
}

void free_all(void) {
    while (c_head) {
        Class *next = c_head->next;
        free(c_head->name);
        Student *s = c_head->stu_head;
        while (s) {
            Student *nexts = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = nexts;
        }
        free(c_head);
        c_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                create_class(name);
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cname[256], sname[256], note[1024];
            if (sscanf(line, "%*s %255s %255s %1023[^\n]", cname, sname, note) == 3) {
                add_student(cname, sname, note);
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], sname[256];
            if (sscanf(line, "%*s %255s %255s %255s", from, to, sname) == 3) {
                transfer_student(from, to, sname);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cname[256], sname[256];
            if (sscanf(line, "%*s %255s %255s", cname, sname) == 2) {
                drop_student(cname, sname);
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_class(name);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }

    free(line);
    free_all();
    return 0;
}