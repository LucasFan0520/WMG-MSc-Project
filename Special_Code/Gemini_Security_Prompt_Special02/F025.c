// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    size_t s_count;
    size_t s_cap;
} Class;

int main() {
    Class *classes = NULL;
    size_t c_count = 0;
    size_t c_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            if (c_count >= c_cap) {
                c_cap = c_cap == 0 ? 4 : c_cap * 2;
                classes = realloc(classes, c_cap * sizeof(Class));
            }
            classes[c_count].name = mystrdup(arg1);
            classes[c_count].students = NULL;
            classes[c_count].s_count = 0;
            classes[c_count].s_cap = 0;
            c_count++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, arg1) == 0) {
                    if (classes[i].s_count >= classes[i].s_cap) {
                        classes[i].s_cap = classes[i].s_cap == 0 ? 4 : classes[i].s_cap * 2;
                        classes[i].students = realloc(classes[i].students, classes[i].s_cap * sizeof(Student));
                    }
                    classes[i].students[classes[i].s_count].name = mystrdup(student);
                    classes[i].students[classes[i].s_count].note = mystrdup(note);
                    classes[i].s_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ') p++;
            char *toClass = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Class *src = NULL;
            Class *dst = NULL;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, arg1) == 0) src = &classes[i];
                if (strcmp(classes[i].name, toClass) == 0) dst = &classes[i];
            }
            if (src && dst) {
                for (size_t j = 0; j < src->s_count; j++) {
                    if (strcmp(src->students[j].name, student) == 0) {
                        if (dst->s_count >= dst->s_cap) {
                            dst->s_cap = dst->s_cap == 0 ? 4 : dst->s_cap * 2;
                            dst->students = realloc(dst->students, dst->s_cap * sizeof(Student));
                        }
                        dst->students[dst->s_count] = src->students[j];
                        dst->s_count++;
                        for (size_t k = j; k < src->s_count - 1; k++) {
                            src->students[k] = src->students[k + 1];
                        }
                        src->s_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, arg1) == 0) {
                    for (size_t j = 0; j < classes[i].s_count; j++) {
                        if (strcmp(classes[i].students[j].name, student) == 0) {
                            free(classes[i].students[j].name);
                            free(classes[i].students[j].note);
                            for (size_t k = j; k < classes[i].s_count - 1; k++) {
                                classes[i].students[k] = classes[i].students[k + 1];
                            }
                            classes[i].s_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, arg1) == 0) {
                    free(classes[i].name);
                    for (size_t j = 0; j < classes[i].s_count; j++) {
                        free(classes[i].students[j].name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (size_t j = i; j < c_count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    c_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (size_t j = 0; j < classes[i].s_count; j++) {
                    printf("  Student: %s Note: %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < c_count; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].s_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
