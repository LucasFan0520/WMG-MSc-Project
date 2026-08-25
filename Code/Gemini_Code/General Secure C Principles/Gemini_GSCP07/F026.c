// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Note *notes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Note *nn = realloc(notes, capacity * sizeof(Note));
                if (!nn) break;
                notes = nn;
            }
            notes[count].category = strdup(cat);
            notes[count].text = strdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, cat) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(notes[i].category, cat) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *word = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("[%s] %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%s] %s\n", notes[i].category, notes[i].text);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
