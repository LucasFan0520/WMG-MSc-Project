// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *student_name;
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
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name_start = line + 6;
            while (*name_start == ' ') name_start++;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name_start) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Class *new_c = realloc(classes, capacity * sizeof(Class));
                    if (new_c) classes = new_c;
                }
                if (count < capacity) {
                    classes[count].name = strdup(name_start);
                    classes[count].students = NULL;
                    classes[count].student_count = 0;
                    classes[count].student_capacity = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *c_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *s_name = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, c_name) == 0) {
                            Class *cl = &classes[i];
                            if (cl->student_count >= cl->student_capacity) {
                                cl->student_capacity = cl->student_capacity == 0 ? 4 : cl->student_capacity * 2;
                                Student *new_s = realloc(cl->students, cl->student_capacity * sizeof(Student));
                                if (new_s) cl->students = new_s;
                            }
                            if (cl->student_count < cl->student_capacity) {
                                cl->students[cl->student_count].student_name = strdup(s_name);
                                cl->students[cl->student_count].note = strdup(note);
                                cl->student_count++;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *from_c = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *to_c = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *s_name = p + 1;
                    while (*s_name == ' ') s_name++;
                    int idx_from = -1, idx_to = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, from_c) == 0) idx_from = i;
                        if (strcmp(classes[i].name, to_c) == 0) idx_to = i;
                    }
                    if (idx_from != -1 && idx_to != -1) {
                        Class *cf = &classes[idx_from];
                        Class *ct = &classes[idx_to];
                        for (int j = 0; j < cf->student_count; j++) {
                            if (strcmp(cf->students[j].student_name, s_name) == 0) {
                                if (ct->student_count >= ct->student_capacity) {
                                    ct->student_capacity = ct->student_capacity == 0 ? 4 : ct->student_capacity * 2;
                                    Student *new_s = realloc(ct->students, ct->student_capacity * sizeof(Student));
                                    if (new_s) ct->students = new_s;
                                }
                                if (ct->student_count < ct->student_capacity) {
                                    ct->students[ct->student_count] = cf->students[j];
                                    ct->student_count++;
                                    for (int k = j; k < cf->student_count - 1; k++) {
                                        cf->students[k] = cf->students[k + 1];
                                    }
                                    cf->student_count--;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *c_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *s_name = p + 1;
                while (*s_name == ' ') s_name++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, c_name) == 0) {
                        Class *cl = &classes[i];
                        for (int j = 0; j < cl->student_count; j++) {
                            if (strcmp(cl->students[j].student_name, s_name) == 0) {
                                free(cl->students[j].student_name);
                                free(cl->students[j].note);
                                for (int k = j; k < cl->student_count - 1; k++) {
                                    cl->students[k] = cl->students[k + 1];
                                }
                                cl->student_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name_start = line + 12;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name_start) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].student_count; j++) {
                        free(classes[i].students[j].student_name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (int j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].student_count; j++) {
                    printf("  %s: %s\n", classes[i].students[j].student_name, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].student_count; j++) {
            free(classes[i].students[j].student_name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
