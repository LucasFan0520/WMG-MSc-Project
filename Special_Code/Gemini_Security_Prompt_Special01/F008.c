// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main(void) {
    Student *gradebook = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *scr_s = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                scr_s++;
            }
            while (*scr_s == ' ') scr_s++;
            char *scr_end = scr_s;
            while (*scr_end && *scr_end != ' ') scr_end++;
            char *note = scr_end;
            if (*scr_end != '\0') {
                *scr_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            int score = atoi(scr_s);
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Student *new_gb = realloc(gradebook, capacity * sizeof(Student));
                if (!new_gb) {
                    free(line);
                    break;
                }
                gradebook = new_gb;
            }
            gradebook[count].name = mystrdup(name);
            gradebook[count].score = score;
            gradebook[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *scr_s = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                scr_s++;
            }
            while (*scr_s == ' ') scr_s++;
            char *scr_end = scr_s;
            while (*scr_end && *scr_end != ' ') scr_end++;
            char *note = scr_end;
            if (*scr_end != '\0') {
                *scr_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            int score = atoi(scr_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(gradebook[i].name, name) == 0) {
                    gradebook[i].score = score;
                    free(gradebook[i].note);
                    gradebook[i].note = mystrdup(note);
                    break;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(gradebook[i].name, name) == 0) {
                    free(gradebook[i].name);
                    free(gradebook[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        gradebook[j] = gradebook[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                int total = 0;
                for (size_t i = 0; i < count; i++) {
                    total += gradebook[i].score;
                }
                printf("%d\n", total / (int)count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    return 0;
}
