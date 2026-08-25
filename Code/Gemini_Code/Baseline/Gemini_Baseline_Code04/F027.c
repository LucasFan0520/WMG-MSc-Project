// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Reminder *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *pri_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *title = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    arr = realloc(arr, (count + 1) * sizeof(Reminder));
                    arr[count].priority = atoi(pri_s);
                    arr[count].title = strdup(title);
                    arr[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                int delta = atoi(p + 1);
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        arr[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            int n = atoi(line + 4);
            if (n > 0 && count > 0) {
                Reminder **temp = malloc(count * sizeof(Reminder *));
                for (int i = 0; i < count; i++) {
                    temp[i] = &arr[i];
                }
                qsort(temp, count, sizeof(Reminder *), compare_reminders);
                int limit = (n < count) ? n : count;
                for (int i = 0; i < limit; i++) {
                    printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                }
                free(temp);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", arr[i].priority, arr[i].title, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
