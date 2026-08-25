// F008.c
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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int score = 0;
            int bytes_parsed = 0;
            if (sscanf(args, "%255s %d%n", name, &score, &bytes_parsed) == 2) {
                char *note = args + bytes_parsed;
                while (*note == ' ') note++;
                gradebook = realloc(gradebook, (count + 1) * sizeof(Student));
                gradebook[count].name = strdup(name);
                gradebook[count].score = score;
                gradebook[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256];
            int score = 0;
            int bytes_parsed = 0;
            if (sscanf(args, "%255s %d%n", name, &score, &bytes_parsed) == 2) {
                char *note = args + bytes_parsed;
                while (*note == ' ') note++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(gradebook[i].name, name) == 0) {
                        gradebook[i].score = score;
                        free(gradebook[i].note);
                        gradebook[i].note = strdup(note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(gradebook[i].name, name) == 0) {
                        free(gradebook[i].name);
                        free(gradebook[i].note);
                        for (int j = i; j < count - 1; j++) {
                            gradebook[j] = gradebook[j + 1];
                        }
                        count--;
                        gradebook = realloc(gradebook, count * sizeof(Student));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
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
        }
    }
    for (int i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    return 0;
}
