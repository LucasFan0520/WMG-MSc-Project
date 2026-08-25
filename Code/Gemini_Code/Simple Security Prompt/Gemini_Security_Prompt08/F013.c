// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char *note;
    int status; 
} Task;

int main(void) {
    Task *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Task *nb = realloc(list, cap * sizeof(Task));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].title = strdup(title);
                list[count].note = strdup(note);
                list[count].status = 0;
                count++;
            }
        } else if (strcmp(cmd, "START") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    list[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    list[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    free(list[i].title);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    char *status_str = list[i].status == 0 ? "TODO" : (list[i].status == 1 ? "DOING" : "DONE");
                    printf("%s %s\n", status_str, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < count; i++) {
                if (list[i].status == 0) printf("%s %s\n", list[i].title, list[i].note);
            }
            printf("DOING:\n");
            for (size_t i = 0; i < count; i++) {
                if (list[i].status == 1) printf("%s %s\n", list[i].title, list[i].note);
            }
            printf("DONE:\n");
            for (size_t i = 0; i < count; i++) {
                if (list[i].status == 2) printf("%s %s\n", list[i].title, list[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        free(list[i].note);
    }
    free(list);
    return 0;
}
