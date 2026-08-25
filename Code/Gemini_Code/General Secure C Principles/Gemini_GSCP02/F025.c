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
    int scount;
} Class;

int main(void) {
    Class *classes = NULL;
    int ccount = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') name++;
            int found = 0;
            for (int i = 0; i < ccount; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Class *tmp = realloc(classes, (ccount + 1) * sizeof(Class));
                if (tmp) {
                    classes = tmp;
                    classes[ccount].name = strdup(name);
                    classes[ccount].students = NULL;
                    classes[ccount].scount = 0;
                    ccount++;
                }
            }
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char cname[256], sname[256];
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", cname, sname) == 2) {
                char *note = strchr(p, ' ');
                if (note) {
                    note = strchr(note + 1, ' ');
                    if (note) {
                        while (*note == ' ') note++;
                    }
                }
                if (!note) note = "";
                for (int i = 0; i < ccount; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        Student *stmp = realloc(classes[i].students, (classes[i].scount + 1) * sizeof(Student));
                        if (stmp) {
                            classes[i].students = stmp;
                            classes[i].students[classes[i].scount].name = strdup(sname);
                            classes[i].students[classes[i].scount].note = strdup(note);
                            classes[i].scount++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char fromC[256], toC[256], sname[256];
            if (sscanf(line + 9, "%255s %255s %255s", fromC, toC, sname) == 3) {
                int fidx = -1, tidx = -1;
                for (int i = 0; i < ccount; i++) {
                    if (strcmp(classes[i].name, fromC) == 0) fidx = i;
                    if (strcmp(classes[i].name, toC) == 0) tidx = i;
                }
                if (fidx != -1 && tidx != -1) {
                    for (int j = 0; j < classes[fidx].scount; j++) {
                        if (strcmp(classes[fidx].students[j].name, sname) == 0) {
                            Student *stmp = realloc(classes[tidx].students, (classes[tidx].scount + 1) * sizeof(Student));
                            if (stmp) {
                                classes[tidx].students = stmp;
                                classes[tidx].students[classes[tidx].scount] = classes[fidx].students[j];
                                classes[tidx].scount++;
                                for (int k = j; k < classes[fidx].scount - 1; k++) {
                                    classes[fidx].students[k] = classes[fidx].students[k + 1];
                                }
                                classes[fidx].scount--;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char cname[256], sname[256];
            if (sscanf(line + 5, "%255s %255s", cname, sname) == 2) {
                for (int i = 0; i < ccount; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        for (int j = 0; j < classes[i].scount; j++) {
                            if (strcmp(classes[i].students[j].name, sname) == 0) {
                                free(classes[i].students[j].name);
                                free(classes[i].students[j].note);
                                for (int k = j; k < classes[i].scount - 1; k++) {
                                    classes[i].students[k] = classes[i].students[k + 1];
                                }
                                classes[i].scount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            while (*name == ' ') name++;
            for (int i = 0; i < ccount; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].scount; j++) {
                        free(classes[i].students[j].name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (int j = i; j < ccount - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    ccount--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].scount; j++) {
                    printf("  Student: %s - %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    free(line);
    for (int i = 0; i < ccount; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].scount; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
