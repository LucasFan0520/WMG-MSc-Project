// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main() {
    Student *gradebook = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                int total = 0;
                for (int i = 0; i < count; i++) {
                    total += gradebook[i].score;
                }
                printf("%d\n", total / count);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int score = 0;
            int n2 = 0;
            if (sscanf(p, "%255s %d%n", name, &score, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    gradebook = realloc(gradebook, capacity * sizeof(Student));
                }
                gradebook[count].name = strdup(name);
                gradebook[count].score = score;
                gradebook[count].note = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256];
            int score = 0;
            int n2 = 0;
            if (sscanf(p, "%255s %d%n", name, &score, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(gradebook[i].name, name) == 0) {
                        gradebook[i].score = score;
                        free(gradebook[i].note);
                        gradebook[i].note = strdup(p);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
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
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    return 0;
}
