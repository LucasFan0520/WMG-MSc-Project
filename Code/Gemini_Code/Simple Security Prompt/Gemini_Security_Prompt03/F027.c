// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

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

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = *(Reminder **)a;
    Reminder *rB = *(Reminder **)b;
    return rB->priority - rA->priority;
}

int main(void) {
    Reminder *list = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *pri_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            list = realloc(list, (count + 1) * sizeof(Reminder));
            list[count].priority = atoi(pri_s);
            list[count].title = strdup(title);
            list[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int delta = atoi(p);
            for (int i = 0; i < count; i++) {
                if (list[i].title && strcmp(list[i].title, title) == 0) {
                    list[i].priority += delta;
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
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p == ' ') p++;
            int n = atoi(p);
            Reminder **temp = malloc(count * sizeof(Reminder *));
            int valid_count = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].title) {
                    temp[valid_count++] = &list[i];
                }
            }
            if (valid_count > 0) {
                qsort(temp, valid_count, sizeof(Reminder *), compare_reminders);
                int limit = (n < valid_count) ? n : valid_count;
                for (int i = 0; i < limit; i++) {
                    printf("%d %s %s\n", temp[i]->priority, temp[i]->title, temp[i]->note);
                }
            }
            free(temp);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].title) {
                    printf("%d %s %s\n", list[i].priority, list[i].title, list[i].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].title) {
            free(list[i].title);
            free(list[i].note);
        }
    }
    free(list);
    return 0;
}
