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

void add_s(const char *name, int score, const char *note) {
    int idx = find_student(name);
    if (idx != -1) {
        gradebook[idx].score = score;
        free(gradebook[idx].note);
        gradebook[idx].note = strdup(note);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        gradebook = realloc(gradebook, capacity * sizeof(Student));
    }
    gradebook[count].name = strdup(name);
    gradebook[count].score = score;
    gradebook[count].note = strdup(note);
    count++;
}

void update_s(const char *name, int score, const char *note) {
    int idx = find_student(name);
    if (idx != -1) {
        gradebook[idx].score = score;
        free(gradebook[idx].note);
        gradebook[idx].note = strdup(note);
    }
}

void drop_s(const char *name) {
    int idx = find_student(name);
    if (idx != -1) {
        free(gradebook[idx].name);
        free(gradebook[idx].note);
        for (int i = idx; i < count - 1; i++) {
            gradebook[i] = gradebook[i + 1];
        }
        count--;
    }
}

void average_s(void) {
    if (count == 0) {
        printf("0\n");
        return;
    }
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += gradebook[i].score;
    }
    printf("%d\n", sum / count);
}

void list_s(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
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
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *score_s = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && score_s && note) {
                add_s(name, atoi(score_s), note);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(NULL, " ");
            char *score_s = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && score_s && note) {
                update_s(name, atoi(score_s), note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) drop_s(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_s();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_s();
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
