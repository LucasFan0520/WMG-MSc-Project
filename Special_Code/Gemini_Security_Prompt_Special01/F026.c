// F026.c
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
    char *category;
    char *text;
} Note;

int main(void) {
    Note *notes = NULL;
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
            char *cat = p;
            while (*cat == ' ') cat++;
            char *cat_end = cat;
            while (*cat_end && *cat_end != ' ') cat_end++;
            char *text = cat_end;
            if (*cat_end != '\0') {
                *cat_end = '\0';
                text++;
            }
            while (*text == ' ') text++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Note *new_n = realloc(notes, capacity * sizeof(Note));
                if (new_n) notes = new_n;
            }
            notes[count].category = mystrdup(cat);
            notes[count].text = mystrdup(text);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = p;
            while (*cat == ' ') cat++;
            char *cat_end = cat;
            while (*cat_end && *cat_end != ' ') cat_end++;
            *cat_end = '\0';
            for (size_t i = 0; i < count; ) {
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
            char *cat = p;
            while (*cat == ' ') cat++;
            char *cat_end = cat;
            while (*cat_end && *cat_end != ' ') cat_end++;
            *cat_end = '\0';
            for (size_t i = 0; i < count; ) {
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
            char *word = p;
            while (*word == ' ') word++;
            char *word_end = word;
            while (*word_end && *word_end != ' ') word_end++;
            *word_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strstr(notes[i].text, word) != NULL) {
                    printf("%s %s\n", notes[i].category, notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
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
