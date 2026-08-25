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

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Class *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            arr = realloc(arr, (count + 1) * sizeof(Class));
            arr[count].name = strdup(name);
            arr[count].students = NULL;
            arr[count].student_count = 0;
            count++;
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *stud = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, cls) == 0) {
                            int sc = arr[i].student_count;
                            arr[i].students = realloc(arr[i].students, (sc + 1) * sizeof(Student));
                            arr[i].students[sc].name = strdup(stud);
                            arr[i].students[sc].note = strdup(note);
                            arr[i].student_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            char *fromClass = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *toClass = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *stud = p + 1;
                    int idxFrom = -1, idxTo = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, fromClass) == 0) idxFrom = i;
                        if (strcmp(arr[i].name, toClass) == 0) idxTo = i;
                    }
                    if (idxFrom != -1 && idxTo != -1) {
                        for (int j = 0; j < arr[idxFrom].student_count; j++) {
                            if (strcmp(arr[idxFrom].students[j].name, stud) == 0) {
                                int sc = arr[idxTo].student_count;
                                arr[idxTo].students = realloc(arr[idxTo].students, (sc + 1) * sizeof(Student));
                                arr[idxTo].students[sc] = arr[idxFrom].students[j];
                                arr[idxTo].student_count++;
                                for (int k = j; k < arr[idxFrom].student_count - 1; k++) {
                                    arr[idxFrom].students[k] = arr[idxFrom].students[k + 1];
                                }
                                arr[idxFrom].student_count--;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *stud = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, cls) == 0) {
                        for (int j = 0; j < arr[i].student_count; j++) {
                            if (strcmp(arr[i].students[j].name, stud) == 0) {
                                free(arr[i].students[j].name);
                                free(arr[i].students[j].note);
                                for (int k = j; k < arr[i].student_count - 1; k++) {
                                    arr[i].students[k] = arr[i].students[k + 1];
                                }
                                arr[i].student_count--;
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
                if (strcmp(arr[i].name, cls) == 0) {
                    free(arr[i].name);
                    for (int j = 0; j < arr[i].student_count; j++) {
                        free(arr[i].students[j].name);
                        free(arr[i].students[j].note);
                    }
                    free(arr[i].students);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("CLASS %s\n", arr[i].name);
                for (int j = 0; j < arr[i].student_count; j++) {
                    printf("STUDENT %s %s\n", arr[i].students[j].name, arr[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].student_count; j++) {
            free(arr[i].students[j].name);
            free(arr[i].students[j].note);
        }
        free(arr[i].students);
    }
    free(arr);
    return 0;
}
