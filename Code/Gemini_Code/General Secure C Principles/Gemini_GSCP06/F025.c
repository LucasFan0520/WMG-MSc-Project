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
    size_t std_count;
    size_t std_capacity;
} Class;

int main(void) {
    Class *classes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Class *next = realloc(classes, capacity * sizeof(Class));
                if (!next) break;
                classes = next;
            }
            classes[count].name = strdup(name);
            classes[count].students = NULL;
            classes[count].std_count = 0;
            classes[count].std_capacity = 0;
            count++;
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *args = line + 8;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *cname = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *sname = rest;
                    char *note = sp2 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, cname) == 0) {
                            if (classes[i].std_count >= classes[i].std_capacity) {
                                classes[i].std_capacity = classes[i].std_capacity == 0 ? 4 : classes[i].std_capacity * 2;
                                Student *nxt = realloc(classes[i].students, classes[i].std_capacity * sizeof(Student));
                                if (!nxt) break;
                                classes[i].students = nxt;
                            }
                            classes[i].students[classes[i].std_count].name = strdup(sname);
                            classes[i].students[classes[i].std_count].note = strdup(note);
                            classes[i].std_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *args = line + 9;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *fromC = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *toC = rest;
                    char *sname = sp2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, fromC) == 0) f_idx = (int)i;
                        if (strcmp(classes[i].name, toC) == 0) t_idx = (int)i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        for (size_t k = 0; k < classes[f_idx].std_count; k++) {
                            if (strcmp(classes[f_idx].students[k].name, sname) == 0) {
                                Student moved = classes[f_idx].students[k];
                                for (size_t j = k; j < classes[f_idx].std_count - 1; j++) {
                                    classes[f_idx].students[j] = classes[f_idx].students[j + 1];
                                }
                                classes[f_idx].std_count--;
                                if (classes[t_idx].std_count >= classes[t_idx].std_capacity) {
                                    classes[t_idx].std_capacity = classes[t_idx].std_capacity == 0 ? 4 : classes[t_idx].std_capacity * 2;
                                    Student *nxt = realloc(classes[t_idx].students, classes[t_idx].std_capacity * sizeof(Student));
                                    if (nxt) classes[t_idx].students = nxt;
                                }
                                classes[t_idx].students[classes[t_idx].std_count++] = moved;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *cname = args;
                char *sname = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        for (size_t k = 0; k < classes[i].std_count; k++) {
                            if (strcmp(classes[i].students[k].name, sname) == 0) {
                                free(classes[i].students[k].name);
                                free(classes[i].students[k].note);
                                for (size_t j = k; j < classes[i].std_count - 1; j++) {
                                    classes[i].students[j] = classes[i].students[j + 1];
                                }
                                classes[i].std_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (size_t k = 0; k < classes[i].std_count; k++) {
                        free(classes[i].students[k].name);
                        free(classes[i].students[k].note);
                    }
                    free(classes[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("CLASS %s\n", classes[i].name);
                for (size_t k = 0; k < classes[i].std_count; k++) {
                    printf("STUDENT %s %s\n", classes[i].students[k].name, classes[i].students[k].note);
                }
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(classes[i].name);
        for (size_t k = 0; k < classes[i].std_count; k++) {
            free(classes[i].students[k].name);
            free(classes[i].students[k].note);
        }
        free(classes[i].students);
    }
    free(classes);
    free(line);
    return 0;
}
