// F013.c
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

typedef struct Task {
    char *title;
    char *note;
    int status;
    struct Task *next;
} Task;

Task *head = NULL;

Task *find_task(const char *title) {
    for (Task *t = head; t; t = t->next) {
        if (strcmp(t->title, title) == 0) return t;
    }
    return NULL;
}

void add_task(const char *title, const char *note, int status) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->status = status;
    t->next = NULL;
    if (!head) head = t;
    else {
        Task *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = t;
    }
}

void move_task(const char *title, int new_status) {
    Task *t = find_task(title);
    if (t) t->status = new_status;
}

void remove_task(const char *title) {
    Task *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_print_task(const char *title) {
    Task *t = find_task(title);
    if (t) {
        const char *list_name = t->status == 0 ? "todo" : (t->status == 1 ? "doing" : "done");
        printf("%s %s\n", list_name, t->note);
    }
}

void report_tasks() {
    const char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        printf("%s:\n", names[i]);
        for (Task *t = head; t; t = t->next) {
            if (t->status == i) {
                printf("  %s: %s\n", t->title, t->note);
            }
        }
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, title[256] = {0};
        sscanf(line, "%15s %255s", cmd, title);
        char *note = strchr(line, ' ');
        if (note) {
            note++;
            while (*note == ' ') note++;
            note = strchr(note, ' ');
            if (note) {
                note++;
                while (*note == ' ') note++;
            } else {
                note = NULL;
            }
        }
        
        if (strcmp(cmd, "TODO") == 0 && note) {
            add_task(title, note, 0);
        } else if (strcmp(cmd, "START") == 0) {
            move_task(title, 1);
        } else if (strcmp(cmd, "DONE") == 0) {
            move_task(title, 2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_print_task(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
        free(line);
    }
    
    Task *t = head;
    while (t) {
        Task *next = t->next;
        free(t->title);
        free(t->note);
        free(t);
        t = next;
    }
    return 0;
}