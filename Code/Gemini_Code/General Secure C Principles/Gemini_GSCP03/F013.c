// F013.c
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
    char *title;
    char *note;
    int status;
} Task;

int main() {
    Task *items = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (int i = 0; i < count; i++) {
                if (items[i].status == 0) printf("%s %s\n", items[i].title, items[i].note);
            }
            printf("DOING:\n");
            for (int i = 0; i < count; i++) {
                if (items[i].status == 1) printf("%s %s\n", items[i].title, items[i].note);
            }
            printf("DONE:\n");
            for (int i = 0; i < count; i++) {
                if (items[i].status == 2) printf("%s %s\n", items[i].title, items[i].note);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Task *nb = realloc(items, cap * sizeof(Task));
                if (nb) items = nb;
            }
            items[count].title = strdup(title);
            items[count].note = strdup(note);
            items[count].status = 0;
            count++;
        } else if (strcmp(cmd, "START") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0 && items[i].status == 0) {
                    items[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0 && items[i].status == 1) {
                    items[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    free(items[i].title);
                    free(items[i].note);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    if (items[i].status == 0) printf("TODO %s\n", items[i].note);
                    else if (items[i].status == 1) printf("DOING %s\n", items[i].note);
                    else if (items[i].status == 2) printf("DONE %s\n", items[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
    }
    free(items);
    return 0;
}
