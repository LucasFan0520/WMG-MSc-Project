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
    int student_count;
} Class;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int main(void) {
    Class *classes = NULL;
    int class_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            classes = realloc(classes, (class_count + 1) * sizeof(Class));
            classes[class_count].name = strdup(name);
            classes[class_count].students = NULL;
            classes[class_count].student_count = 0;
            class_count++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            for (int i = 0; i < class_count; i++) {
                if (classes[i].name && strcmp(classes[i].name, cname) == 0) {
                    int sc = classes[i].student_count;
                    classes[i].students = realloc(classes[i].students, (sc + 1) * sizeof(Student));
                    classes[i].students[sc].name = strdup(sname);
                    classes[i].students[sc].note = strdup(note);
                    classes[i].student_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ') p++;
            char *fromC = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *toC = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idxFrom = -1, idxTo = -1;
            for (int i = 0; i < class_count; i++) {
                if (classes[i].name && strcmp(classes[i].name, fromC) == 0) idxFrom = i;
                if (classes[i].name && strcmp(classes[i].name, toC) == 0) idxTo = i;
            }
            if (idxFrom != -1 && idxTo != -1) {
                for (int j = 0; j < classes[idxFrom].student_count; j++) {
                    if (strcmp(classes[idxFrom].students[j].name, sname) == 0) {
                        int scTo = classes[idxTo].student_count;
                        classes[idxTo].students = realloc(classes[idxTo].students, (scTo + 1) * sizeof(Student));
                        classes[idxTo].students[scTo] = classes[idxFrom].students[j];
                        classes[idxTo].student_count++;
                        for (int k = j; k < classes[idxFrom].student_count - 1; k++) {
                            classes[idxFrom].students[k] = classes[idxFrom].students[k + 1];
                        }
                        classes[idxFrom].student_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < class_count; i++) {
                if (classes[i].name && strcmp(classes[i].name, cname) == 0) {
                    for (int j = 0; j < classes[i].student_count; j++) {
                        if (strcmp(classes[i].students[j].name, sname) == 0) {
                            free(classes[i].students[j].name);
                            free(classes[i].students[j].note);
                            for (int k = j; k < classes[i].student_count - 1; k++) {
                                classes[i].students[k] = classes[i].students[k + 1];
                            }
                            classes[i].student_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < class_count; i++) {
                if (classes[i].name && strcmp(classes[i].name, cname) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].student_count; j++) {
                        free(classes[i].students[j].name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    classes[i].name = NULL;
                    classes[i].students = NULL;
                    classes[i].student_count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < class_count; i++) {
                if (classes[i].name) {
                    printf("Class: %s\n", classes[i].name);
                    for (int j = 0; j < classes[i].student_count; j++) {
                        printf("  Student: %s Note: %s\n", classes[i].students[j].name, classes[i].students[j].note);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < class_count; i++) {
        if (classes[i].name) {
            free(classes[i].name);
            for (int j = 0; j < classes[i].student_count; j++) {
                free(classes[i].students[j].name);
                free(classes[i].students[j].note);
            }
            free(classes[i].students);
        }
    }
    free(classes);
    return 0;
}
