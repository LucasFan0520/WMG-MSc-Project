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
    int sc;
} Class;

Class classes[1000];
int cc = 0;

int find_class(char *name) {
    for (int i = 0; i < cc; i++) if (strcmp(classes[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "CLASS") == 0) {
            if (find_class(rest) == -1) {
                classes[cc].name = strdup(rest);
                classes[cc].students = NULL;
                classes[cc].sc = 0;
                cc++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[1000], name[1000];
            sscanf(rest, "%s %s", cls, name);
            char *note = rest;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            int idx = find_class(cls);
            if (idx != -1) {
                classes[idx].students = realloc(classes[idx].students, sizeof(Student) * (classes[idx].sc + 1));
                classes[idx].students[classes[idx].sc].name = strdup(name);
                classes[idx].students[classes[idx].sc].note = strdup(note);
                classes[idx].sc++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[1000], to[1000], name[1000];
            sscanf(rest, "%s %s %s", from, to, name);
            int ifr = find_class(from), ito = find_class(to);
            if (ifr != -1 && ito != -1) {
                for (int i = 0; i < classes[ifr].sc; i++) {
                    if (strcmp(classes[ifr].students[i].name, name) == 0) {
                        classes[ito].students = realloc(classes[ito].students, sizeof(Student) * (classes[ito].sc + 1));
                        classes[ito].students[classes[ito].sc] = classes[ifr].students[i];
                        classes[ito].sc++;
                        for (int j = i; j < classes[ifr].sc - 1; j++) classes[ifr].students[j] = classes[ifr].students[j+1];
                        classes[ifr].sc--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[1000], name[1000];
            sscanf(rest, "%s %s", cls, name);
            int idx = find_class(cls);
            if (idx != -1) {
                for (int i = 0; i < classes[idx].sc; i++) {
                    if (strcmp(classes[idx].students[i].name, name) == 0) {
                        free(classes[idx].students[i].name);
                        free(classes[idx].students[i].note);
                        for (int j = i; j < classes[idx].sc - 1; j++) classes[idx].students[j] = classes[idx].students[j+1];
                        classes[idx].sc--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            int idx = find_class(rest);
            if (idx != -1) {
                free(classes[idx].name);
                for (int i = 0; i < classes[idx].sc; i++) {
                    free(classes[idx].students[i].name);
                    free(classes[idx].students[i].note);
                }
                free(classes[idx].students);
                for (int i = idx; i < cc - 1; i++) classes[i] = classes[i+1];
                cc--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < cc; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].sc; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < cc; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].sc; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    return 0;
}