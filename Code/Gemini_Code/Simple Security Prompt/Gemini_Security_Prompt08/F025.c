// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *student_name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    size_t std_count;
    size_t std_cap;
} Class;

int main(void) {
    Class *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Class *nb = realloc(list, cap * sizeof(Class));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].name = strdup(arg1);
                list[count].students = NULL;
                list[count].std_count = 0;
                list[count].std_cap = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, cname) == 0) {
                    if (list[i].std_count >= list[i].std_cap) {
                        list[i].std_cap = list[i].std_cap == 0 ? 4 : list[i].std_cap * 2;
                        Student *ns = realloc(list[i].students, list[i].std_cap * sizeof(Student));
                        if (ns) list[i].students = ns;
                    }
                    if (list[i].std_count < list[i].std_cap) {
                        list[i].students[list[i].std_count].student_name = strdup(student);
                        list[i].students[list[i].std_count].note = strdup(note);
                        list[i].std_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromClass = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *toClass = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t f_idx = (size_t)-1;
            size_t t_idx = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, fromClass) == 0) f_idx = i;
                if (strcmp(list[i].name, toClass) == 0) t_idx = i;
            }
            if (f_idx != (size_t)-1 && t_idx != (size_t)-1) {
                for (size_t j = 0; j < list[f_idx].std_count; j++) {
                    if (strcmp(list[f_idx].students[j].student_name, student) == 0) {
                        if (list[t_idx].std_count >= list[t_idx].std_cap) {
                            list[t_idx].std_cap = list[t_idx].std_cap == 0 ? 4 : list[t_idx].std_cap * 2;
                            Student *ns = realloc(list[t_idx].students, list[t_idx].std_cap * sizeof(Student));
                            if (ns) list[t_idx].students = ns;
                        }
                        if (list[t_idx].std_count < list[t_idx].std_cap) {
                            list[t_idx].students[list[t_idx].std_count] = list[f_idx].students[j];
                            list[t_idx].std_count++;
                            for (size_t k = j; k < list[f_idx].std_count - 1; k++) {
                                list[f_idx].students[k] = list[f_idx].students[k + 1];
                            }
                            list[f_idx].std_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, cname) == 0) {
                    for (size_t j = 0; j < list[i].std_count; j++) {
                        if (strcmp(list[i].students[j].student_name, student) == 0) {
                            free(list[i].students[j].student_name);
                            free(list[i].students[j].note);
                            for (size_t k = j; k < list[i].std_count - 1; k++) {
                                list[i].students[k] = list[i].students[k + 1];
                            }
                            list[i].std_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    for (size_t j = 0; j < list[i].std_count; j++) {
                        free(list[i].students[j].student_name);
                        free(list[i].students[j].note);
                    }
                    free(list[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Class: %s\n", list[i].name);
                for (size_t j = 0; j < list[i].std_count; j++) {
                    printf("- %s: %s\n", list[i].students[j].student_name, list[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        for (size_t j = 0; j < list[i].std_count; j++) {
            free(list[i].students[j].student_name);
            free(list[i].students[j].note);
        }
        free(list[i].students);
    }
    free(list);
    return 0;
}
