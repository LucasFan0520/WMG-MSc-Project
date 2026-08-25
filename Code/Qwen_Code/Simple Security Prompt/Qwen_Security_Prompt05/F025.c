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
    int num_students;
} Class;

Class *classes = NULL;
int num_classes = 0;

int find_class(const char *name) {
    for (int i = 0; i < num_classes; i++) {
        if (strcmp(classes[i].name, name) == 0) return i;
    }
    return -1;
}

int find_student(Class *c, const char *name) {
    for (int i = 0; i < c->num_students; i++) {
        if (strcmp(c->students[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            classes = realloc(classes, (num_classes + 1) * sizeof(Class));
            classes[num_classes].name = strdup(name);
            classes[num_classes].students = NULL;
            classes[num_classes].num_students = 0;
            num_classes++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256], stu[256];
            sscanf(line, "%*s %s %s", cls, stu);
            char *note = strstr(line, stu);
            if (note) {
                note += strlen(stu);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            int cidx = find_class(cls);
            if (cidx >= 0) {
                classes[cidx].students = realloc(classes[cidx].students, (classes[cidx].num_students + 1) * sizeof(Student));
                classes[cidx].students[classes[cidx].num_students].name = strdup(stu);
                classes[cidx].students[classes[cidx].num_students].note = strdup(note);
                classes[cidx].num_students++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], stu[256];
            sscanf(line, "%*s %s %s %s", from, to, stu);
            int cidx_from = find_class(from);
            int cidx_to = find_class(to);
            if (cidx_from >= 0 && cidx_to >= 0) {
                int sidx = find_student(&classes[cidx_from], stu);
                if (sidx >= 0) {
                    classes[cidx_to].students = realloc(classes[cidx_to].students, (classes[cidx_to].num_students + 1) * sizeof(Student));
                    classes[cidx_to].students[classes[cidx_to].num_students] = classes[cidx_from].students[sidx];
                    classes[cidx_to].num_students++;
                    
                    for (int i = sidx; i < classes[cidx_from].num_students - 1; i++) {
                        classes[cidx_from].students[i] = classes[cidx_from].students[i + 1];
                    }
                    classes[cidx_from].num_students--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256], stu[256];
            sscanf(line, "%*s %s %s", cls, stu);
            int cidx = find_class(cls);
            if (cidx >= 0) {
                int sidx = find_student(&classes[cidx], stu);
                if (sidx >= 0) {
                    free(classes[cidx].students[sidx].name);
                    free(classes[cidx].students[sidx].note);
                    for (int i = sidx; i < classes[cidx].num_students - 1; i++) {
                        classes[cidx].students[i] = classes[cidx].students[i + 1];
                    }
                    classes[cidx].num_students--;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int cidx = find_class(name);
            if (cidx >= 0) {
                for (int i = 0; i < classes[cidx].num_students; i++) {
                    free(classes[cidx].students[i].name);
                    free(classes[cidx].students[i].note);
                }
                free(classes[cidx].students);
                free(classes[cidx].name);
                for (int i = cidx; i < num_classes - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                num_classes--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < num_classes; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].num_students; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    
    for (int i = 0; i < num_classes; i++) {
        for (int j = 0; j < classes[i].num_students; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
        free(classes[i].name);
    }
    free(classes);
    
    return 0;
}