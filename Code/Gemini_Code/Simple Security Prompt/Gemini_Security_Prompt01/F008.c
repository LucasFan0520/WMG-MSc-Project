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

int main() {
    Student *gradebook = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *score_str = space2 + 1;
                while (*score_str == ' ') score_str++;
                char *space3 = strchr(score_str, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                int score = atoi(score_str);
                gradebook = realloc(gradebook, (count + 1) * sizeof(Student));
                if (!gradebook) return 1;
                gradebook[count].name = strdup(name);
                gradebook[count].score = score;
                gradebook[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *score_str = space2 + 1;
                while (*score_str == ' ') score_str++;
                char *space3 = strchr(score_str, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                int score = atoi(score_str);
                for (int i = 0; i < count; i++) {
                    if (strcmp(gradebook[i].name, name) == 0) {
                        gradebook[i].score = score;
                        free(gradebook[i].note);
                        gradebook[i].note = strdup(note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(gradebook[i].name, args) == 0) {
                    free(gradebook[i].name);
                    free(gradebook[i].note);
                    for (int j = i; j < count - 1; j++) {
                        gradebook[j] = gradebook[j + 1];
                    }
                    count--;
                    gradebook = realloc(gradebook, count * sizeof(Student));
                    if (count > 0 && !gradebook) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long total = 0;
                for (int i = 0; i < count; i++) {
                    total += gradebook[i].score;
                }
                printf("%lld\n", total / count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
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
