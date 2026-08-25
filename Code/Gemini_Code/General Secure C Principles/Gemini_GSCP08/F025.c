// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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

typedef struct {
    char *name;
    char *note;
} ClassroomStudent;

typedef struct {
    char *name;
    ClassroomStudent *students;
    size_t std_count;
    size_t std_cap;
} Classroom;

int main(void) {
    Classroom *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("CLASS %s\n", arr[i].name);
                    for (size_t j = 0; j < arr[i].std_count; j++) {
                        printf("STUDENT %s %s\n", arr[i].students[j].name, arr[i].students[j].note);
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = args;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    Classroom *narr = realloc(arr, ncap * sizeof(Classroom));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].name = strdup(name);
                    arr[count].students = NULL;
                    arr[count].std_count = 0;
                    arr[count].std_cap = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cls = args;
            char *p2 = strchr(cls, ' ');
            if (p2) {
                *p2 = '\0';
                char *student = p2 + 1;
                char *p3 = strchr(student, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, cls) == 0) {
                            Classroom *c = &arr[i];
                            if (c->std_count >= c->std_cap) {
                                size_t ncap = c->std_cap == 0 ? 4 : c->std_cap * 2;
                                ClassroomStudent *nstuds = realloc(c->students, ncap * sizeof(ClassroomStudent));
                                if (nstuds) {
                                    c->students = nstuds;
                                    c->std_cap = ncap;
                                }
                            }
                            if (c->std_count < c->std_cap) {
                                c->students[c->std_count].name = strdup(student);
                                c->students[c->std_count].note = strdup(note);
                                c->std_count++;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromC = args;
            char *p2 = strchr(fromC, ' ');
            if (p2) {
                *p2 = '\0';
                char *toC = p2 + 1;
                char *p3 = strchr(toC, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *student = p3 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, fromC) == 0) f_idx = (int)i;
                        if (strcmp(arr[i].name, toC) == 0) t_idx = (int)i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        Classroom *fc = &arr[f_idx];
                        Classroom *tc = &arr[t_idx];
                        for (size_t j = 0; j < fc->std_count; j++) {
                            if (strcmp(fc->students[j].name, student) == 0) {
                                if (tc->std_count >= tc->std_cap) {
                                    size_t ncap = tc->std_cap == 0 ? 4 : tc->std_cap * 2;
                                    ClassroomStudent *nstuds = realloc(tc->students, ncap * sizeof(ClassroomStudent));
                                    if (nstuds) {
                                        tc->students = nstuds;
                                        tc->std_cap = ncap;
                                    }
                                }
                                if (tc->std_count < tc->std_cap) {
                                    tc->students[tc->std_count] = fc->students[j];
                                    tc->std_count++;
                                    for (size_t k = j; k < fc->std_count - 1; k++) {
                                        fc->students[k] = fc->students[k + 1];
                                    }
                                    fc->std_count--;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cls = args;
            char *p2 = strchr(cls, ' ');
            if (p2) {
                *p2 = '\0';
                char *student = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, cls) == 0) {
                        Classroom *c = &arr[i];
                        for (size_t j = 0; j < c->std_count; j++) {
                            if (strcmp(c->students[j].name, student) == 0) {
                                free(c->students[j].name);
                                free(c->students[j].note);
                                for (size_t k = j; k < c->std_count - 1; k++) {
                                    c->students[k] = c->students[k + 1];
                                }
                                c->std_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].std_count; j++) {
                        free(arr[i].students[j].name);
                        free(arr[i].students[j].note);
                    }
                    free(arr[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].std_count; j++) {
            free(arr[i].students[j].name);
            free(arr[i].students[j].note);
        }
        free(arr[i].students);
    }
    free(arr);
    return 0;
}
