// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

int main() {
    Student *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", items[i].name, items[i].score, items[i].note);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                int sum = 0;
                for (size_t i = 0; i < count; i++) {
                    sum += items[i].score;
                }
                printf("%d\n", sum / (int)count);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *scr_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Student *nb = realloc(items, cap * sizeof(Student));
                if (nb) items = nb;
            }
            items[count].name = strdup(name);
            items[count].score = atoi(scr_str);
            items[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *scr_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    items[i].score = atoi(scr_str);
                    free(items[i].note);
                    items[i].note = strdup(note);
                    break;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    return 0;
}
