// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    int s_count;
    int s_capacity;
} Class;

Class *classes = NULL;
int c_count = 0;
int c_capacity = 0;

int find_class(const char *name) {
    for (int i = 0; i < c_count; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_class(const char *name) {
    if (find_class(name) != -1) {
        return;
    }
    if (c_count >= c_capacity) {
        c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
        classes = realloc(classes, c_capacity * sizeof(Class));
    }
    classes[c_count].name = strdup(name);
    classes[c_count].students = NULL;
    classes[c_count].s_count = 0;
    classes[c_count].s_capacity = 0;
    c_count++;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            add_class(line + 6);
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p1 = line + 8;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *note = sp2 + 1;
                    int c_idx = find_class(p1);
                    if (c_idx != -1) {
                        Class *c = &classes[c_idx];
                        if (c->s_count >= c->s_capacity) {
                            c->s_capacity = c->s_capacity == 0 ? 4 : c->s_capacity * 2;
                            c->students = realloc(c->students, c->s_capacity * sizeof(Student));
                        }
                        c->students[c->s_count].name = strdup(p2);
                        c->students[c->s_count].note = strdup(note);
                        c->s_count++;
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p1 = line + 9;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *student = sp2 + 1;
                    int idx_from = find_class(p1);
                    int idx_to = find_class(p2);
                    if (idx_from != -1 && idx_to != -1) {
                        Class *cf = &classes[idx_from];
                        Class *ct = &classes[idx_to];
                        int s_idx = -1;
                        for (int i = 0; i < cf->s_count; i++) {
                            if (strcmp(cf->students[i].name, student) == 0) {
                                s_idx = i;
                                break;
                            }
                        }
                        if (s_idx != -1) {
                            if (ct->s_count >= ct->s_capacity) {
                                ct->s_capacity = ct->s_capacity == 0 ? 4 : ct->s_capacity * 2;
                                ct->students = realloc(ct->students, ct->s_capacity * sizeof(Student));
                            }
                            ct->students[ct->s_count] = cf->students[s_idx];
                            ct->s_count++;
                            for (int i = s_idx; i < cf->s_count - 1; i++) {
                                cf->students[i] = cf->students[i + 1];
                            }
                            cf->s_count--;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p1 = line + 5;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *student = sp + 1;
                int c_idx = find_class(p1);
                if (c_idx != -1) {
                    Class *c = &classes[c_idx];
                    for (int i = 0; i < c->s_count; i++) {
                        if (strcmp(c->students[i].name, student) == 0) {
                            free(c->students[i].name);
                            free(c->students[i].note);
                            for (int j = i; j < c->s_count - 1; j++) {
                                c->students[j] = c->students[j + 1];
                            }
                            c->s_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            int c_idx = find_class(name);
            if (c_idx != -1) {
                free(classes[c_idx].name);
                for (int i = 0; i < classes[c_idx].s_count; i++) {
                    free(classes[c_idx].students[i].name);
                    free(classes[c_idx].students[i].note);
                }
                free(classes[c_idx].students);
                for (int i = c_idx; i < c_count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                c_count--;
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("CLASS %s\n", classes[i].name);
                for (int j = 0; j < classes[i].s_count; j++) {
                    printf("STUDENT %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < c_count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].s_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
