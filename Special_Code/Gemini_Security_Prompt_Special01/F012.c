// F012.c
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
    char *note;
} Person;

int main(void) {
    Person *q = NULL;
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
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *note = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Person *new_q = realloc(q, capacity * sizeof(Person));
                if (new_q) q = new_q;
            }
            q[count].name = mystrdup(name);
            q[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", q[0].name, q[0].note);
                free(q[0].name);
                free(q[0].note);
                for (size_t i = 0; i < count - 1; i++) {
                    q[i] = q[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(q[i].name, name) == 0) {
                    free(q[i].name);
                    free(q[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        q[j] = q[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(q[i].name, name) == 0) {
                    printf("%s\n", q[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", q[i].name, q[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(q[i].name);
        free(q[i].note);
    }
    free(q);
    return 0;
}
