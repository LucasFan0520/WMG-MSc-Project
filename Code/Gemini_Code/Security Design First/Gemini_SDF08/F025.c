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
    int s_count;
} Class;

int main(void) {
    Class *classes = NULL;
    int count = 0;
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
            Class *tmp = realloc(classes, sizeof(Class) * (count + 1));
            if (tmp) {
                classes = tmp;
                classes[count].name = strdup(name);
                classes[count].students = NULL;
                classes[count].s_count = 0;
                if (classes[count].name) {
                    count++;
                }
            }
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *c_name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *s_name = p2;
                    char *note = s2 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, c_name) == 0) {
                            Student *stmp = realloc(classes[i].students, sizeof(Student) * (classes[i].s_count + 1));
                            if (stmp) {
                                classes[i].students = stmp;
                                classes[i].students[classes[i].s_count].name = strdup(s_name);
                                classes[i].students[classes[i].s_count].note = strdup(note);
                                if (classes[i].students[classes[i].s_count].name && classes[i].students[classes[i].s_count].note) {
                                    classes[i].s_count++;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *fromC = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *toC = p2;
                    char *s_name = s2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, fromC) == 0) f_idx = i;
                        if (strcmp(classes[i].name, toC) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        int s_found = -1;
                        for (int j = 0; j < classes[f_idx].s_count; j++) {
                            if (strcmp(classes[f_idx].students[j].name, s_name) == 0) {
                                s_found = j;
                                break;
                            }
                        }
                        if (s_found != -1) {
                            Student *stmp = realloc(classes[t_idx].students, sizeof(Student) * (classes[t_idx].s_count + 1));
                            if (stmp) {
                                classes[t_idx].students = stmp;
                                classes[t_idx].students[classes[t_idx].s_count] = classes[f_idx].students[s_found];
                                classes[t_idx].s_count++;
                                for (int j = s_found; j < classes[f_idx].s_count - 1; j++) {
                                    classes[f_idx].students[j] = classes[f_idx].students[j + 1];
                                }
                                classes[f_idx].s_count--;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *c_name = p;
                char *s_name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, c_name) == 0) {
                        int s_idx = -1;
                        for (int j = 0; j < classes[i].s_count; j++) {
                            if (strcmp(classes[i].students[j].name, s_name) == 0) {
                                s_idx = j;
                                break;
                            }
                        }
                        if (s_idx != -1) {
                            free(classes[i].students[s_idx].name);
                            free(classes[i].students[s_idx].note);
                            for (int j = s_idx; j < classes[i].s_count - 1; j++) {
                                classes[i].students[j] = classes[i].students[j + 1];
                            }
                            classes[i].s_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(classes[idx].name);
                for (int j = 0; j < classes[idx].s_count; j++) {
                    free(classes[idx].students[j].name);
                    free(classes[idx].students[j].note);
                }
                free(classes[idx].students);
                for (int i = idx; i < count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].s_count; j++) {
                    printf("  Student: %s - %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].s_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    free(line);
    return 0;
}
