// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *student;
    char *note;
} RosterStudent;

typedef struct {
    char *name;
    RosterStudent *students;
    int stud_count;
} Classroom;

int main() {
    Classroom *classes = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    classes = realloc(classes, (count + 1) * sizeof(Classroom));
                    classes[count].name = strdup(name);
                    classes[count].students = NULL;
                    classes[count].stud_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256], stud[256];
            int c_len = 0, s_len = 0;
            if (sscanf(ptr, "%255s%n", cls, &c_len) > 0) {
                char *ptr2 = ptr + c_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", stud, &s_len) > 0) {
                    char *note = ptr2 + s_len;
                    while (*note == ' ') note++;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(classes[i].name, cls) == 0) {
                            int sc = classes[i].stud_count;
                            classes[i].students = realloc(classes[i].students, (sc + 1) * sizeof(RosterStudent));
                            classes[i].students[sc].student = strdup(stud);
                            classes[i].students[sc].note = strdup(note);
                            classes[i].stud_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], stud[256];
            if (sscanf(ptr, "%255s %255s %255s", from, to, stud) == 3) {
                int f_idx = -1, t_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, from) == 0) f_idx = i;
                    if (strcmp(classes[i].name, to) == 0) t_idx = i;
                }
                if (f_idx != -1 && t_idx != -1) {
                    int s_idx = -1;
                    for (int j = 0; j < classes[f_idx].stud_count; j++) {
                        if (strcmp(classes[f_idx].students[j].student, stud) == 0) {
                            s_idx = j;
                            break;
                        }
                    }
                    if (s_idx != -1) {
                        int sc = classes[t_idx].stud_count;
                        classes[t_idx].students = realloc(classes[t_idx].students, (sc + 1) * sizeof(RosterStudent));
                        classes[t_idx].students[sc] = classes[f_idx].students[s_idx];
                        classes[t_idx].stud_count++;
                        for (int j = s_idx; j < classes[f_idx].stud_count - 1; j++) {
                            classes[f_idx].students[j] = classes[f_idx].students[j + 1];
                        }
                        classes[f_idx].stud_count--;
                        if (classes[f_idx].stud_count > 0) {
                            classes[f_idx].students = realloc(classes[f_idx].students, classes[f_idx].stud_count * sizeof(RosterStudent));
                        } else {
                            free(classes[f_idx].students);
                            classes[f_idx].students = NULL;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256], stud[256];
            if (sscanf(ptr, "%255s %255s", cls, stud) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cls) == 0) {
                        int f_idx = -1;
                        for (int j = 0; j < classes[i].stud_count; j++) {
                            if (strcmp(classes[i].students[j].student, stud) == 0) {
                                f_idx = j;
                                break;
                            }
                        }
                        if (f_idx != -1) {
                            free(classes[i].students[f_idx].student);
                            free(classes[i].students[f_idx].note);
                            for (int j = f_idx; j < classes[i].stud_count - 1; j++) {
                                classes[i].students[j] = classes[i].students[j + 1];
                            }
                            classes[i].stud_count--;
                            if (classes[i].stud_count > 0) {
                                classes[i].students = realloc(classes[i].students, classes[i].stud_count * sizeof(RosterStudent));
                            } else {
                                free(classes[i].students);
                                classes[i].students = NULL;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(classes[found].name);
                    for (int j = 0; j < classes[found].stud_count; j++) {
                        free(classes[found].students[j].student);
                        free(classes[found].students[j].note);
                    }
                    free(classes[found].students);
                    for (int i = found; i < count - 1; i++) {
                        classes[i] = classes[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        classes = realloc(classes, count * sizeof(Classroom));
                    } else {
                        free(classes);
                        classes = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].stud_count; j++) {
                    printf("  Student: %s Note: %s\n", classes[i].students[j].student, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].stud_count; j++) {
            free(classes[i].students[j].student);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
