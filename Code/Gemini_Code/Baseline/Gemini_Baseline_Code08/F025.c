// F025.c
#define _GNU_SOURCE
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
    int std_count;
    int std_capacity;
} Class;

Class *classes = NULL;
int class_count = 0;
int class_capacity = 0;

int find_class(const char *name) {
    for (int i = 0; i < class_count; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_class(const char *name) {
    if (find_class(name) != -1) return;
    if (class_count >= class_capacity) {
        class_capacity = class_capacity == 0 ? 4 : class_capacity * 2;
        classes = realloc(classes, class_capacity * sizeof(Class));
    }
    classes[class_count].name = strdup(name);
    classes[class_count].students = NULL;
    classes[class_count].std_count = 0;
    classes[class_count].std_capacity = 0;
    class_count++;
}

void add_student(const char *c_name, const char *std_name, const char *note) {
    int idx = find_class(c_name);
    if (idx == -1) return;
    Class *c = &classes[idx];
    if (c->std_count >= c->std_capacity) {
        c->std_capacity = c->std_capacity == 0 ? 4 : c->std_capacity * 2;
        c->students = realloc(c->students, c->std_capacity * sizeof(Student));
    }
    c->students[c->std_count].name = strdup(std_name);
    c->students[c->std_count].note = strdup(note);
    c->std_count++;
}

void transfer_student(const char *from_c, const char *to_c, const char *std_name) {
    int idxF = find_class(from_c);
    int idxT = find_class(to_c);
    if (idxF == -1 || idxT == -1) return;
    Class *cF = &classes[idxF];
    Class *cT = &classes[idxT];
    int s_idx = -1;
    for (int i = 0; i < cF->std_count; i++) {
        if (strcmp(cF->students[i].name, std_name) == 0) {
            s_idx = i;
            break;
        }
    }
    if (s_idx == -1) return;
    if (cT->std_count >= cT->std_capacity) {
        cT->std_capacity = cT->std_capacity == 0 ? 4 : cT->std_capacity * 2;
        cT->students = realloc(cT->students, cT->std_capacity * sizeof(Student));
    }
    cT->students[cT->std_count] = cF->students[s_idx];
    cT->std_count++;
    for (int i = s_idx; i < cF->std_count - 1; i++) {
        cF->students[i] = cF->students[i + 1];
    }
    cF->std_count--;
}

void drop_student(const char *c_name, const char *std_name) {
    int idx = find_class(c_name);
    if (idx == -1) return;
    Class *c = &classes[idx];
    for (int i = 0; i < c->std_count; i++) {
        if (strcmp(c->students[i].name, std_name) == 0) {
            free(c->students[i].name);
            free(c->students[i].note);
            for (int j = i; j < c->std_count - 1; j++) {
                c->students[j] = c->students[j + 1];
            }
            c->std_count--;
            break;
        }
    }
}

void delete_class(const char *name) {
    int idx = find_class(name);
    if (idx == -1) return;
    free(classes[idx].name);
    for (int i = 0; i < classes[idx].std_count; i++) {
        free(classes[idx].students[i].name);
        free(classes[idx].students[i].note);
    }
    free(classes[idx].students);
    for (int i = idx; i < class_count - 1; i++) {
        classes[i] = classes[i + 1];
    }
    class_count--;
}

void print_classes(void) {
    for (int i = 0; i < class_count; i++) {
        printf("CLASS %s\n", classes[i].name);
        for (int j = 0; j < classes[i].std_count; j++) {
            printf("STUDENT %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
        }
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = strtok(NULL, "");
            if (name) create_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *c_name = strtok(NULL, " ");
            char *std_name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (c_name && std_name && note) add_student(c_name, std_name, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from_c = strtok(NULL, " ");
            char *to_c = strtok(NULL, " ");
            char *std_name = strtok(NULL, "");
            if (from_c && to_c && std_name) transfer_student(from_c, to_c, std_name);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *c_name = strtok(NULL, " ");
            char *std_name = strtok(NULL, "");
            if (c_name && std_name) drop_student(c_name, std_name);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free(line);
    for (int i = 0; i < class_count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].std_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
