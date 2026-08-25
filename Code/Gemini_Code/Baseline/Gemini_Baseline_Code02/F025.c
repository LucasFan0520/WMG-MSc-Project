// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *stud_name;
    char *note;
} Stud;

typedef struct {
    char *name;
    Stud *studs;
    int stud_count;
} Class;

int main() {
    Class *classes = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("Class: %s\n", classes[i].name);
                    for (int j = 0; j < classes[i].stud_count; j++) {
                        printf("  %s: %s\n", classes[i].studs[j].stud_name, classes[i].studs[j].note);
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "CLASS") == 0) {
            classes = realloc(classes, sizeof(Class) * (count + 1));
            classes[count].name = strdup(args);
            classes[count].studs = NULL;
            classes[count].stud_count = 0;
            count++;
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
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, cls) == 0) {
                            int sc = classes[i].stud_count;
                            classes[i].studs = realloc(classes[i].studs, sizeof(Stud) * (sc + 1));
                            classes[i].studs[sc].stud_name = strdup(student);
                            classes[i].studs[sc].note = strdup(note);
                            classes[i].stud_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromClass = args;
            char *p2 = strchr(fromClass, ' ');
            if (p2) {
                *p2 = '\0';
                char *toClass = p2 + 1;
                char *p3 = strchr(toClass, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *student = p3 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, fromClass) == 0) f_idx = i;
                        if (strcmp(classes[i].name, toClass) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        for (int j = 0; j < classes[f_idx].stud_count; j++) {
                            if (strcmp(classes[f_idx].studs[j].stud_name, student) == 0) {
                                int sc = classes[t_idx].stud_count;
                                classes[t_idx].studs = realloc(classes[t_idx].studs, sizeof(Stud) * (sc + 1));
                                classes[t_idx].studs[sc] = classes[f_idx].studs[j];
                                classes[t_idx].stud_count++;
                                for (int k = j; k < classes[f_idx].stud_count - 1; k++) {
                                    classes[f_idx].studs[k] = classes[f_idx].studs[k + 1];
                                }
                                classes[f_idx].stud_count--;
                                if (classes[f_idx].stud_count == 0) {
                                    free(classes[f_idx].studs);
                                    classes[f_idx].studs = NULL;
                                } else {
                                    classes[f_idx].studs = realloc(classes[f_idx].studs, sizeof(Stud) * classes[f_idx].stud_count);
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
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cls) == 0) {
                        for (int j = 0; j < classes[i].stud_count; j++) {
                            if (strcmp(classes[i].studs[j].stud_name, student) == 0) {
                                free(classes[i].studs[j].stud_name);
                                free(classes[i].studs[j].note);
                                for (int k = j; k < classes[i].stud_count - 1; k++) {
                                    classes[i].studs[k] = classes[i].studs[k + 1];
                                }
                                classes[i].stud_count--;
                                if (classes[i].stud_count == 0) {
                                    free(classes[i].studs);
                                    classes[i].studs = NULL;
                                } else {
                                    classes[i].studs = realloc(classes[i].studs, sizeof(Stud) * classes[i].stud_count);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(classes[i].name, args) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].stud_count; j++) {
                        free(classes[i].studs[j].stud_name);
                        free(classes[i].studs[j].note);
                    }
                    free(classes[i].studs);
                    for (int j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    if (count == 0) { free(classes); classes = NULL; }
                    else { classes = realloc(classes, sizeof(Class) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].stud_count; j++) {
            free(classes[i].studs[j].stud_name);
            free(classes[i].studs[j].note);
        }
        free(classes[i].studs);
    }
    free(classes);
    return 0;
}
