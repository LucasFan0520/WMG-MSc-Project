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
    int stud_count;
    int stud_cap;
} ClassRoster;

ClassRoster *classes = NULL;
int class_count = 0;
int class_cap = 0;

int find_class(const char *name) {
    for (int i = 0; i < class_count; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "CLASS") == 0) {
                if (find_class(arg1) == -1) {
                    if (class_count >= class_cap) {
                        class_cap = class_cap == 0 ? 4 : class_cap * 2;
                        ClassRoster *tmp = realloc(classes, class_cap * sizeof(ClassRoster));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        classes = tmp;
                    }
                    classes[class_count].name = strdup(arg1);
                    classes[class_count].students = NULL;
                    classes[class_count].stud_count = 0;
                    classes[class_count].stud_cap = 0;
                    if (classes[class_count].name) {
                        class_count++;
                    }
                }
            } else if (strcmp(cmd, "STUDENT") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *stud_name = space2 + 1;
                    char *space3 = strchr(stud_name, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *note = space3 + 1;
                        int idx = find_class(arg1);
                        if (idx != -1) {
                            ClassRoster *c = &classes[idx];
                            if (c->stud_count >= c->stud_cap) {
                                c->stud_cap = c->stud_cap == 0 ? 4 : c->stud_cap * 2;
                                Student *tmp = realloc(c->students, c->stud_cap * sizeof(Student));
                                if (!tmp) {
                                    free(line);
                                    return 1;
                                }
                                c->students = tmp;
                            }
                            c->students[c->stud_count].name = strdup(stud_name);
                            c->students[c->stud_count].note = strdup(note);
                            if (c->students[c->stud_count].name && c->students[c->stud_count].note) {
                                c->stud_count++;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "TRANSFER") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *to_class = space2 + 1;
                    char *space3 = strchr(to_class, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *stud_name = space3 + 1;
                        int idx_from = find_class(arg1);
                        int idx_to = find_class(to_class);
                        if (idx_from != -1 && idx_to != -1 && idx_from != idx_to) {
                            ClassRoster *cf = &classes[idx_from];
                            ClassRoster *ct = &classes[idx_to];
                            for (int i = 0; i < cf->stud_count; i++) {
                                if (strcmp(cf->students[i].name, stud_name) == 0) {
                                    if (ct->stud_count >= ct->stud_cap) {
                                        ct->stud_cap = ct->stud_cap == 0 ? 4 : ct->stud_cap * 2;
                                        Student *tmp = realloc(ct->students, ct->stud_cap * sizeof(Student));
                                        if (!tmp) {
                                            free(line);
                                            return 1;
                                        }
                                        ct->students = tmp;
                                    }
                                    ct->students[ct->stud_count] = cf->students[i];
                                    ct->stud_count++;
                                    for (int j = i; j < cf->stud_count - 1; j++) {
                                        cf->students[j] = cf->students[j + 1];
                                    }
                                    cf->stud_count--;
                                    break;
                                }
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DROP") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *stud_name = space2 + 1;
                    int idx = find_class(arg1);
                    if (idx != -1) {
                        ClassRoster *c = &classes[idx];
                        for (int i = 0; i < c->stud_count; i++) {
                            if (strcmp(c->students[i].name, stud_name) == 0) {
                                free(c->students[i].name);
                                free(c->students[i].note);
                                for (int j = i; j < c->stud_count - 1; j++) {
                                    c->students[j] = c->students[j + 1];
                                }
                                c->stud_count--;
                                break;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETECLASS") == 0) {
                int idx = find_class(arg1);
                if (idx != -1) {
                    free(classes[idx].name);
                    for (int i = 0; i < classes[idx].stud_count; i++) {
                        free(classes[idx].students[i].name);
                        free(classes[idx].students[i].note);
                    }
                    free(classes[idx].students);
                    for (int i = idx; i < class_count - 1; i++) {
                        classes[i] = classes[i + 1];
                    }
                    class_count--;
                }
            }
        } else {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < class_count; i++) {
                    printf("Class %s:\n", classes[i].name);
                    for (int j = 0; j < classes[i].stud_count; j++) {
                        printf("  Student: %s - %s\n", classes[i].students[j].name, classes[i].students[j].note);
                    }
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < class_count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].stud_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    classes = NULL;
    return 0;
}