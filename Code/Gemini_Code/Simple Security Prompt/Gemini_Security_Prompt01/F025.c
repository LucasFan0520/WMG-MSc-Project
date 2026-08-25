// F025.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} StudentInfo;

typedef struct {
    char *name;
    StudentInfo *students;
    int student_count;
} ClassInfo;

int main() {
    ClassInfo *classes = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "CLASS") == 0 && args) {
            classes = realloc(classes, (count + 1) * sizeof(ClassInfo));
            if (!classes) return 1;
            classes[count].name = strdup(args);
            classes[count].students = NULL;
            classes[count].student_count = 0;
            count++;
        } else if (strcmp(cmd, "STUDENT") == 0 && args) {
            char *cname = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *sname = space2 + 1;
                while (*sname == ' ') sname++;
                char *space3 = strchr(sname, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        int sc = classes[i].student_count;
                        classes[i].students = realloc(classes[i].students, (sc + 1) * sizeof(StudentInfo));
                        if (!classes[i].students) return 1;
                        classes[i].students[sc].name = strdup(sname);
                        classes[i].students[sc].note = strdup(note);
                        classes[i].student_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0 && args) {
            char *fromC = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *toC = space2 + 1;
                while (*toC == ' ') toC++;
                char *space3 = strchr(toC, ' ');
                if (space3) {
                    *space3 = '\0';
                    char *sname = space3 + 1;
                    while (*sname == ' ') sname++;

                    int f_idx = -1, t_idx = -1, s_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, fromC) == 0) f_idx = i;
                        if (strcmp(classes[i].name, toC) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        for (int j = 0; j < classes[f_idx].student_count; j++) {
                            if (strcmp(classes[f_idx].students[j].name, sname) == 0) {
                                s_idx = j;
                                break;
                            }
                        }
                        if (s_idx != -1) {
                            int sc = classes[t_idx].student_count;
                            classes[t_idx].students = realloc(classes[t_idx].students, (sc + 1) * sizeof(StudentInfo));
                            if (!classes[t_idx].students) return 1;
                            classes[t_idx].students[sc] = classes[f_idx].students[s_idx];
                            classes[t_idx].student_count++;

                            for (int j = s_idx; j < classes[f_idx].student_count - 1; j++) {
                                classes[f_idx].students[j] = classes[f_idx].students[j + 1];
                            }
                            classes[f_idx].student_count--;
                            classes[f_idx].students = realloc(classes[f_idx].students, classes[f_idx].student_count * sizeof(StudentInfo));
                            if (classes[f_idx].student_count > 0 && !classes[f_idx].students) return 1;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0 && args) {
            char *cname = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *sname = space2 + 1;
                while (*sname == ' ') sname++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        for (int j = 0; j < classes[i].student_count; j++) {
                            if (strcmp(classes[i].students[j].name, sname) == 0) {
                                free(classes[i].students[j].name);
                                free(classes[i].students[j].note);
                                for (int k = j; k < classes[i].student_count - 1; k++) {
                                    classes[i].students[k] = classes[i].students[k + 1];
                                }
                                classes[i].student_count--;
                                classes[i].students = realloc(classes[i].students, classes[i].student_count * sizeof(StudentInfo));
                                if (classes[i].student_count > 0 && !classes[i].students) return 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, args) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].student_count; j++) {
                        free(classes[i].students[j].name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (int j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    classes = realloc(classes, count * sizeof(ClassInfo));
                    if (count > 0 && !classes) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("CLASS %s\n", classes[i].name);
                for (int j = 0; j < classes[i].student_count; j++) {
                    printf("STUDENT %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
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
