// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *student;
    char *note;
} RosterStudent;

typedef struct {
    char *name;
    RosterStudent *students;
    int student_count;
} Classroom;

int main() {
    Classroom *classes = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            classes = realloc(classes, (count + 1) * sizeof(Classroom));
            classes[count].name = strdup(name);
            classes[count].students = NULL;
            classes[count].student_count = 0;
            count++;
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char cls[128], stud[128];
            if (sscanf(line + 8, "%127s %127s", cls, stud) == 2) {
                char *p = line + 8;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cls) == 0) {
                        classes[i].students = realloc(classes[i].students, (classes[i].student_count + 1) * sizeof(RosterStudent));
                        classes[i].students[classes[i].student_count].student = strdup(stud);
                        classes[i].students[classes[i].student_count].note = strdup(p);
                        classes[i].student_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char fromC[128], toC[128], stud[128];
            if (sscanf(line + 9, "%127s %127s %127s", fromC, toC, stud) == 3) {
                int f_idx = -1, t_idx = -1, s_sub = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, fromC) == 0) f_idx = i;
                    if (strcmp(classes[i].name, toC) == 0) t_idx = i;
                }
                if (f_idx != -1 && t_idx != -1) {
                    for (int j = 0; j < classes[f_idx].student_count; j++) {
                        if (strcmp(classes[f_idx].students[j].student, stud) == 0) {
                            s_sub = j;
                            break;
                        }
                    }
                    if (s_sub != -1) {
                        classes[t_idx].students = realloc(classes[t_idx].students, (classes[t_idx].student_count + 1) * sizeof(RosterStudent));
                        classes[t_idx].students[classes[t_idx].student_count] = classes[f_idx].students[s_sub];
                        classes[t_idx].student_count++;
                        for (int j = s_sub; j < classes[f_idx].student_count - 1; j++) {
                            classes[f_idx].students[j] = classes[f_idx].students[j + 1];
                        }
                        classes[f_idx].student_count--;
                        classes[f_idx].students = realloc(classes[f_idx].students, classes[f_idx].student_count * sizeof(RosterStudent));
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char cls[128], stud[128];
            if (sscanf(line + 5, "%127s %127s", cls, stud) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cls) == 0) {
                        for (int j = 0; j < classes[i].student_count; j++) {
                            if (strcmp(classes[i].students[j].student, stud) == 0) {
                                free(classes[i].students[j].student);
                                free(classes[i].students[j].note);
                                for (int m = j; m < classes[i].student_count - 1; m++) {
                                    classes[i].students[m] = classes[i].students[m + 1];
                                }
                                classes[i].student_count--;
                                classes[i].students = realloc(classes[i].students, classes[i].student_count * sizeof(RosterStudent));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *cls = line + 12;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].student_count; j++) {
                        free(classes[i].students[j].student);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (int j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    classes = realloc(classes, count * sizeof(Classroom));
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].student_count; j++) {
                    printf("  Student: %s Note: %s\n", classes[i].students[j].student, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].student_count; j++) {
            free(classes[i].students[j].student);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
