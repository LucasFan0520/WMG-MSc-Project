// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

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

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
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

void free_class(Class *c) {
    Student *s = c->students;
    while (s) {
        Student *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
    free(c->name);
    free(c);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_class(name)) {
                Class *c = malloc(sizeof(Class));
                if (c) {
                    c->name = strdup(name);
                    c->students = NULL;
                    c->next = head;
                    head = c;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = get_token(&ptr);
            char *sname = get_token(&ptr);
            if (cname && sname) {
                Class *c = find_class(cname);
                if (c && !find_student(c, sname)) {
                    Student *s = malloc(sizeof(Student));
                    if (s) {
                        s->name = strdup(sname);
                        s->note = strdup(ptr);
                        s->next = c->students;
                        c->students = s;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from = get_token(&ptr);
            char *to = get_token(&ptr);
            char *sname = get_token(&ptr);
            if (from && to && sname) {
                Class *cf = find_class(from);
                Class *ct = find_class(to);
                if (cf && ct) {
                    Student *prev = NULL, *curr = cf->students;
                    while (curr) {
                        if (strcmp(curr->name, sname) == 0) {
                            if (prev) prev->next = curr->next;
                            else cf->students = curr->next;
                            curr->next = ct->students;
                            ct->students = curr;
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = get_token(&ptr);
            char *sname = get_token(&ptr);
            if (cname && sname) {
                Class *c = find_class(cname);
                if (c) {
                    Student *prev = NULL, *curr = c->students;
                    while (curr) {
                        if (strcmp(curr->name, sname) == 0) {
                            if (prev) prev->next = curr->next;
                            else c->students = curr->next;
                            free(curr->name);
                            free(curr->note);
                            free(curr);
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Class *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free_class(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Class *c = head; c; c = c->next) {
                printf("%s\n", c->name);
                for (Student *s = c->students; s; s = s->next) {
                    printf("  %s %s\n", s->name, s->note);
                }
            }
        }
    }
    while (head) {
        Class *next = head->next;
        free_class(head);
        head = next;
    }
    return 0;
}