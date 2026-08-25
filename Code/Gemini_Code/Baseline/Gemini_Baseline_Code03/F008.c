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
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int score = 0;
            int n_len = 0, s_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *ptr2 = ptr + n_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%d%n", &score, &s_len) > 0) {
                    char *note = ptr2 + s_len;
                    while (*note == ' ') note++;
                    gradebook = realloc(gradebook, (count + 1) * sizeof(Student));
                    gradebook[count].name = strdup(name);
                    gradebook[count].score = score;
                    gradebook[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256];
            int score = 0;
            int n_len = 0, s_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *ptr2 = ptr + n_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%d%n", &score, &s_len) > 0) {
                    char *note = ptr2 + s_len;
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
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(gradebook[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(gradebook[found].name);
                    free(gradebook[found].note);
                    for (int i = found; i < count - 1; i++) {
                        gradebook[i] = gradebook[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        gradebook = realloc(gradebook, count * sizeof(Student));
                    } else {
                        free(gradebook);
                        gradebook = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count > 0) {
                int sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += gradebook[i].score;
                }
                printf("%d\n", sum / count);
            } else {
                printf("0\n");
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
