// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    return rB->priority - rA->priority;
}

int main() {
    Reminder *reminders = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            int prio;
            char title[128];
            if (sscanf(line + 4, "%d %127s", &prio, title) == 2) {
                char *p = line + 4;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                reminders = realloc(reminders, (count + 1) * sizeof(Reminder));
                reminders[count].priority = prio;
                reminders[count].title = strdup(title);
                reminders[count].note = strdup(p);
                count++;
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char title[128];
            int delta;
            if (sscanf(line + 5, "%127s %d", title, &delta) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(reminders[i].title, title) == 0) {
                        reminders[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    free(reminders[i].title);
                    free(reminders[i].note);
                    for (int j = i; j < count - 1; j++) {
                        reminders[j] = reminders[j + 1];
                    }
                    count--;
                    reminders = realloc(reminders, count * sizeof(Reminder));
                    break;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            int n = atoi(line + 4);
            if (n > count) n = count;
            Reminder *temp = malloc(count * sizeof(Reminder));
            memcpy(temp, reminders, count * sizeof(Reminder));
            qsort(temp, count, sizeof(Reminder), compare_reminders);
            for (int i = 0; i < n; i++) {
                printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
            }
            free(temp);
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}
