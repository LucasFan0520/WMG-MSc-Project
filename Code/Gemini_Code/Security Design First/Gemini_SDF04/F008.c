// F008.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

Student *gradebook = NULL;
int count = 0;
int capacity = 0;

int find_student(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(gradebook[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *score_str = space2 + 1;
                    char *space3 = strchr(score_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        int score = atoi(score_str);
                        char *note = space3 + 1;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Student *tmp = realloc(gradebook, capacity * sizeof(Student));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            gradebook = tmp;
                        }
                        gradebook[count].name = strdup(arg1);
                        gradebook[count].score = score;
                        gradebook[count].note = strdup(note);
                        if (gradebook[count].name && gradebook[count].note) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "UPDATE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *score_str = space2 + 1;
                    char *space3 = strchr(score_str, ' ');
                    if (space3) {
                        *space3 = '\0';
                        int score = atoi(score_str);
                        char *note = space3 + 1;
                        int idx = find_student(arg1);
                        if (idx != -1) {
                            char *nn = strdup(note);
                            if (nn) {
                                free(gradebook[idx].note);
                                gradebook[idx].note = nn;
                                gradebook[idx].score = score;
                            }
                        }
                    }
                }
            } else if (strcmp(cmd, "DROP") == 0) {
                int idx = find_student(arg1);
                if (idx != -1) {
                    free(gradebook[idx].name);
                    free(gradebook[idx].note);
                    for (int i = idx; i < count - 1; i++) {
                        gradebook[i] = gradebook[i + 1];
                    }
                    count--;
                }
            }
        } else {
            if (strcmp(cmd, "AVERAGE") == 0) {
                if (count == 0) {
                    printf("0\n");
                } else {
                    int sum = 0;
                    for (int i = 0; i < count; i++) {
                        sum += gradebook[i].score;
                    }
                    printf("%d\n", sum / count);
                }
            } else if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    return 0;
}