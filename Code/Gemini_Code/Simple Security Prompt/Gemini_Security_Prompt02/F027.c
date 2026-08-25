// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t id;
} Reminder;

int cmp_reminder(const void *a, const void *b) {
    Reminder *pa = *(Reminder **)a;
    Reminder *pb = *(Reminder **)b;
    if (pb->priority != pa->priority) {
        return pb->priority - pa->priority;
    }
    return (pa->id < pb->id) ? -1 : 1;
}

int main() {
    Reminder *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t next_id = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                int priority = atoi(p);
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                char *title = p2;
                char *note = "";
                if (s2) {
                    *s2 = '\0';
                    note = s2 + 1;
                }
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Reminder));
                }
                arr[count].priority = priority;
                arr[count].title = strdup(title);
                arr[count].note = strdup(note);
                arr[count].id = next_id++;
                count++;
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *title = p;
                int delta = atoi(s1 + 1);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].title, title) == 0) {
                        arr[i].priority += delta;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].title);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            int n = atoi(line + 4);
            if (n > (int)count) n = (int)count;
            if (n > 0) {
                Reminder **copies = malloc(count * sizeof(Reminder *));
                for (size_t i = 0; i < count; i++) {
                    copies[i] = &arr[i];
                }
                qsort(copies, count, sizeof(Reminder *), cmp_reminder);
                for (int i = 0; i < n; i++) {
                    printf("%d %s %s\n", copies[i]->priority, copies[i]->title, copies[i]->note);
                }
                free(copies);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%d %s %s\n", arr[i].priority, arr[i].title, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
