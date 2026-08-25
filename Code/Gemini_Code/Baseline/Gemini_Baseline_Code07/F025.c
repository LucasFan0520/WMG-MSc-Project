// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
} Student;

typedef struct {
    char *class_name;
    Student *students;
    int stud_count;
    int stud_capacity;
} Class;

int main() {
    Class *classes = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("CLASS %s\n", classes[i].class_name);
                for (int j = 0; j < classes[i].stud_count; j++) {
                    printf("STUDENT %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    classes = realloc(classes, capacity * sizeof(Class));
                }
                classes[count].class_name = strdup(name);
                classes[count].students = NULL;
                classes[count].stud_count = 0;
                classes[count].stud_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cname[256], sname[256];
            int n2 = 0;
            if (sscanf(p, "%255s %255s%n", cname, sname, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].class_name, cname) == 0) {
                        if (classes[i].stud_count >= classes[i].stud_capacity) {
                            classes[i].stud_capacity = classes[i].stud_capacity == 0 ? 4 : classes[i].stud_capacity * 2;
                            classes[i].students = realloc(classes[i].students, classes[i].stud_capacity * sizeof(Student));
                        }
                        classes[i].students[classes[i].stud_count].name = strdup(sname);
                        classes[i].students[classes[i].stud_count].note = strdup(p);
                        classes[i].stud_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char fromC[256], toC[256], sname[256];
            if (sscanf(p, "%255s %255s %255s", fromC, toC, sname) == 3) {
                int fIdx = -1, tIdx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].class_name, fromC) == 0) fIdx = i;
                    if (strcmp(classes[i].class_name, toC) == 0) tIdx = i;
                }
                if (fIdx != -1 && tIdx != -1) {
                    int found_stud = -1;
                    for (int j = 0; j < classes[fIdx].stud_count; j++) {
                        if (strcmp(classes[fIdx].students[j].name, sname) == 0) {
                            found_stud = j;
                            break;
                        }
                    }
                    if (found_stud != -1) {
                        if (classes[tIdx].stud_count >= classes[tIdx].stud_capacity) {
                            classes[tIdx].stud_capacity = classes[tIdx].stud_capacity == 0 ? 4 : classes[tIdx].stud_capacity * 2;
                            classes[tIdx].students = realloc(classes[tIdx].students, classes[tIdx].stud_capacity * sizeof(Student));
                        }
                        classes[tIdx].students[classes[tIdx].stud_count] = classes[fIdx].students[found_stud];
                        classes[tIdx].stud_count++;
                        for (int j = found_stud; j < classes[fIdx].stud_count - 1; j++) {
                            classes[fIdx].students[j] = classes[fIdx].students[j + 1];
                        }
                        classes[fIdx].stud_count--;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cname[256], sname[256];
            if (sscanf(p, "%255s %255s", cname, sname) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].class_name, cname) == 0) {
                        int found_stud = -1;
                        for (int j = 0; j < classes[i].stud_count; j++) {
                            if (strcmp(classes[i].students[j].name, sname) == 0) {
                                found_stud = j;
                                break;
                            }
                        }
                        if (found_stud != -1) {
                            free(classes[i].students[found_stud].name);
                            free(classes[i].students[found_stud].note);
                            for (int j = found_stud; j < classes[i].stud_count - 1; j++) {
                                classes[i].students[j] = classes[i].students[j + 1];
                            }
                            classes[i].stud_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].class_name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(classes[found].class_name);
                    for (int j = 0; j < classes[found].stud_count; j++) {
                        free(classes[found].students[j].name);
                        free(classes[found].students[j].note);
                    }
                    free(classes[found].students);
                    for (int i = found; i < count - 1; i++) {
                        classes[i] = classes[i + 1];
                    }
                    count--;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].class_name);
        for (int j = 0; j < classes[i].stud_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
