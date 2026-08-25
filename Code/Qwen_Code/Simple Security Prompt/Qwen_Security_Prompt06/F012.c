// F012.c
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

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *head = NULL;
Person *tail = NULL;

void join_queue(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void serve_queue() {
    if (head) {
        printf("%s\n", head->name);
        Person *tmp = head;
        head = head->next;
        if (!head) tail = NULL;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

void cancel_person(const char *name) {
    Person *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_person(const char *name) {
    for (Person *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            printf("%s\n", p->note);
            return;
        }
    }
}

void print_queue() {
    for (Person *p = head; p; p = p->next) {
        printf("%s\n", p->name);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, name[256] = {0};
        sscanf(line, "%15s %255s", cmd, name);
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
        
        if (strcmp(cmd, "JOIN") == 0 && note) {
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_queue();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0 && note) {
            join_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_person(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
        free(line);
    }
    
    Person *p = head;
    while (p) {
        Person *next = p->next;
        free(p->name);
        free(p->note);
        free(p);
        p = next;
    }
    return 0;
}