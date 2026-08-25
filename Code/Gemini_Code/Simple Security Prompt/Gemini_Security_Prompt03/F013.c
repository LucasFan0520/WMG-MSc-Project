// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int status; // 0=TODO, 1=DOING, 2=DONE
} Task;

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

int main(void) {
    Task *list = NULL;
    int count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TODO") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            list = realloc(list, (count + 1) * sizeof(Task));
            list[count].title = strdup(title);
            list[count].note = strdup(note);
            list[count].status = 0;
            count++;
        } else if (strcmp(cmd, "START") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0 && list[i].status == 0) {
                    list[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0 && list[i].status == 1) {
                    list[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0) {
                    free(list[i].title);
                    free(list[i].note);
                    list[i].title = NULL;
                    list[i].note = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0) {
                    const char *st = (list[i].status == 0) ? "TODO" : ((list[i].status == 1) ? "DOING" : "DONE");
                    printf("%s %s\n", st, list[i].note);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("--- TODO ---\n");
            for (int i = 0; i < count; i++) {
                if (list[i].title && list[i].status == 0) printf("%s: %s\n", list[i].title, list[i].note);
            }
            printf("--- DOING ---\n");
            for (int i = 0; i < count; i++) {
                if (list[i].title && list[i].status == 1) printf("%s: %s\n", list[i].title, list[i].note);
            }
            printf("--- DONE ---\n");
            for (int i = 0; i < count; i++) {
                if (list[i].title && list[i].status == 2) printf("%s: %s\n", list[i].title, list[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].title) free(list[i].title);
        if (list[i].note) free(list[i].note);
    }
    free(list);
    return 0;
}
