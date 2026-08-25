// F025.c
#define _GNU_SOURCE
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
    int student_count;
    int student_capacity;
} Class;

int main(void) {
    Class *classes = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') name++;
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Class *tmp = realloc(classes, new_cap * sizeof(Class));
                if (!tmp) return 1;
                classes = tmp;
                capacity = new_cap;
            }
            classes[count].name = strdup(name);
            classes[count].students = NULL;
            classes[count].student_count = 0;
            classes[count].student_capacity = 0;
            count++;
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *cname = p;
            char *space1 = strchr(cname, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *sname = space1 + 1;
            while (*sname == ' ') sname++;
            char *space2 = strchr(sname, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            int c_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    c_idx = i;
                    break;
                }
            }
            if (c_idx != -1) {
                Class *c = &classes[c_idx];
                if (c->student_count >= c->student_capacity) {
                    int new_scap = c->student_capacity == 0 ? 4 : c->student_capacity * 2;
                    Student *tmp = realloc(c->students, new_scap * sizeof(Student));
                    if (!tmp) return 1;
                    c->students = tmp;
                    c->student_capacity = new_scap;
                }
                c->students[c->student_count].name = strdup(sname);
                c->students[c->student_count].note = strdup(note);
                c->student_count++;
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *fromC = p;
            char *space1 = strchr(fromC, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *toC = space1 + 1;
            while (*toC == ' ') toC++;
            char *space2 = strchr(toC, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *sname = space2 + 1;
            while (*sname == ' ') sname++;
            
            int f_idx = -1, t_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, fromC) == 0) f_idx = i;
                if (strcmp(classes[i].name, toC) == 0) t_idx = i;
            }
            if (f_idx != -1 && t_idx != -1) {
                Class *fc = &classes[f_idx];
                Class *tc = &classes[t_idx];
                int s_idx = -1;
                for (int j = 0; j < fc->student_count; j++) {
                    if (strcmp(fc->students[j].name, sname) == 0) {
                        s_idx = j;
                        break;
                    }
                }
                if (s_idx != -1) {
                    if (tc->student_count >= tc->student_capacity) {
                        int new_scap = tc->student_capacity == 0 ? 4 : tc->student_capacity * 2;
                        Student *tmp = realloc(tc->students, new_scap * sizeof(Student));
                        if (!tmp) return 1;
                        tc->students = tmp;
                        tc->student_capacity = new_scap;
                    }
                    tc->students[tc->student_count] = fc->students[s_idx];
                    tc->student_count++;
                    for (int j = s_idx; j < fc->student_count - 1; j++) {
                        fc->students[j] = fc->students[j + 1];
                    }
                    fc->student_count--;
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *cname = p;
            char *space = strchr(cname, ' ');
            if (!space) continue;
            *space = '\0';
            char *sname = space + 1;
            while (*sname == ' ') sname++;
            int c_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    c_idx = i;
                    break;
                }
            }
            if (c_idx != -1) {
                Class *c = &classes[c_idx];
                int s_idx = -1;
                for (int j = 0; j < c->student_count; j++) {
                    if (strcmp(c->students[j].name, sname) == 0) {
                        s_idx = j;
                        break;
                    }
                }
                if (s_idx != -1) {
                    free(c->students[s_idx].name);
                    free(c->students[s_idx].note);
                    for (int j = s_idx; j < c->student_count - 1; j++) {
                        c->students[j] = c->students[j + 1];
                    }
                    c->student_count--;
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            while (*name == ' ') name++;
            int c_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    c_idx = i;
                    break;
                }
            }
            if (c_idx != -1) {
                free(classes[c_idx].name);
                for (int j = 0; j < classes[c_idx].student_count; j++) {
                    free(classes[c_idx].students[j].name);
                    free(classes[c_idx].students[j].note);
                }
                free(classes[c_idx].students);
                for (int i = c_idx; i < count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].student_count; j++) {
                    printf("%s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].student_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
