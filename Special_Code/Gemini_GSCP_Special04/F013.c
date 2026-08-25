/* F013.c */
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *note;
    int status;
} Task;

int main(void) {
    Task *tasks = NULL;
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TODO") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Task *nt = realloc(tasks, cap * sizeof(Task));
                if (nt) tasks = nt;
            }
            char *ntitle = mystrdup(title);
            char *nnote = mystrdup(note);
            if (ntitle && nnote) {
                tasks[count].title = ntitle;
                tasks[count].note = nnote;
                tasks[count].status = 0;
                count++;
            } else {
                free(ntitle);
                free(nnote);
            }
        } else if (strcmp(cmd, "START") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 0 && strcmp(tasks[i].title, title) == 0) {
                    tasks[i].status = 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 1 && strcmp(tasks[i].title, title) == 0) {
                    tasks[i].status = 2;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(tasks[found].title);
                free(tasks[found].note);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    tasks[i] = tasks[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(tasks[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                const char *st_name = (tasks[found].status == 0) ? "TODO" : ((tasks[found].status == 1) ? "DOING" : "DONE");
                printf("%s %s\n", st_name, tasks[found].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("TODO:\n");
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 0) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DOING:\n");
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 1) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
            printf("DONE:\n");
            for (size_t i = 0; i < count; i++) {
                if (tasks[i].status == 2) printf("%s %s\n", tasks[i].title, tasks[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}
