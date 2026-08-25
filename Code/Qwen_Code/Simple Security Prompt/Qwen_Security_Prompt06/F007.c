// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Activity {
    int time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;

void add_activity(int time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = time;
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (!head) {
        head = a;
    } else {
        Activity *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = a;
    }
}

void delete_activity(int time, const char *label) {
    Activity *prev = NULL, *curr = head;
    while (curr) {
        if (curr->time == time && strcmp(curr->label, label) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->label);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_activity(const char *label) {
    Activity *earliest = NULL;
    for (Activity *a = head; a; a = a->next) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || a->time < earliest->time) {
                earliest = a;
            }
        }
    }
    if (earliest) {
        printf("%d %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

int cmp_activity(const void *a, const void *b) {
    Activity *aa = *(Activity **)a;
    Activity *bb = *(Activity **)b;
    if (aa->time != bb->time) return aa->time - bb->time;
    return strcmp(aa->label, bb->label);
}

void sort_activities() {
    int count = 0;
    for (Activity *a = head; a; a = a->next) count++;
    if (count == 0) return;
    
    Activity **arr = malloc(count * sizeof(Activity *));
    Activity *curr = head;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    qsort(arr, count, sizeof(Activity *), cmp_activity);
    
    for (int i = 0; i < count - 1; i++) {
        arr[i]->next = arr[i+1];
    }
    arr[count-1]->next = NULL;
    head = arr[0];
    free(arr);
    
    for (Activity *a = head; a; a = a->next) {
        printf("%d %s %s\n", a->time, a->label, a->note);
    }
}

void clear_activities() {
    Activity *curr = head;
    while (curr) {
        Activity *next = curr->next;
        free(curr->label);
        free(curr->note);
        free(curr);
        curr = next;
    }
    head = NULL;
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, label[256] = {0};
        int time = 0;
        sscanf(line, "%15s %d %255s", cmd, &time, label);
        char *note = strchr(line, ' ');
        if (note) {
            note++;
            while (*note == ' ') note++;
            note = strchr(note, ' ');
            if (note) {
                note++;
                while (*note == ' ') note++;
                note = strchr(note, ' ');
                if (note) {
                    note++;
                    while (*note == ' ') note++;
                }
            } else {
                note = NULL;
            }
        }
        
        if (strcmp(cmd, "ADD") == 0 && note) {
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_activity(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
        free(line);
    }
    clear_activities();
    return 0;
}