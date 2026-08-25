// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main(void) {
    Student *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "AVERAGE") == 0) {
                if (count == 0) {
                    printf("0\n");
                } else {
                    long total = 0;
                    for (size_t i = 0; i < count; i++) {
                        total += arr[i].score;
                    }
                    printf("%ld\n", total / (long)count);
                }
            } else if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %d %s\n", arr[i].name, arr[i].score, arr[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *s_str = p2 + 1;
                char *p3 = strchr(s_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    int score = (int)strtol(s_str, NULL, 10);
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= cap) {
                            size_t ncap = cap == 0 ? 4 : cap * 2;
                            Student *narr = realloc(arr, ncap * sizeof(Student));
                            if (narr) {
                                arr = narr;
                                cap = ncap;
                            }
                        }
                        if (count < cap) {
                            arr[count].name = strdup(name);
                            arr[count].score = score;
                            arr[count].note = strdup(note);
                            count++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *s_str = p2 + 1;
                char *p3 = strchr(s_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    int score = (int)strtol(s_str, NULL, 10);
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            char *nnote = strdup(note);
                            if (nnote) {
                                free(arr[i].note);
                                arr[i].note = nnote;
                                arr[i].score = score;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = args;
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
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
