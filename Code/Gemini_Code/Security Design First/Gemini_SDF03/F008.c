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

int main(void) {
    Student *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *score_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                score_str = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            int sc = atoi(score_str);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Student *tmp = realloc(arr, capacity * sizeof(Student));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                arr[count].score = sc;
                arr[count].note = strdup(note);
                if (!arr[count].name || !arr[count].note) exit(1);
                count++;
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *score_str = "0";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                score_str = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            int sc = atoi(score_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    arr[i].score = sc;
                    free(arr[i].note);
                    arr[i].note = strdup(note);
                    if (!arr[i].note) exit(1);
                    break;
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long sum = 0;
                for (size_t i = 0; i < count; i++) {
                    sum += arr[i].score;
                }
                printf("%lld\n", sum / (long long)count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", arr[i].name, arr[i].score, arr[i].note);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    free(line);
    return 0;
}
