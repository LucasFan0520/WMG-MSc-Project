// F008.c
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

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

Student *head = NULL;

Student *find_student(const char *name) {
    for (Student *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) {
        s = malloc(sizeof(Student));
        s->name = strdup(name);
        s->note = strdup(note);
        s->score = score;
        s->next = head;
        head = s;
    }
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
    }
}

void drop_student(const char *name) {
    Student *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_average() {
    int sum = 0, count = 0;
    for (Student *s = head; s; s = s->next) {
        sum += s->score;
        count++;
    }
    if (count > 0) printf("%d\n", sum / count);
    else printf("0\n");
}

void list_students() {
    for (Student *s = head; s; s = s->next) {
        printf("%s %d %s\n", s->name, s->score, s->note);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, name[256] = {0};
        int score = 0;
        sscanf(line, "%15s %255s %d", cmd, name, &score);
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
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0 && note) {
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            print_average();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
        free(line);
    }
    
    Student *s = head;
    while (s) {
        Student *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
    return 0;
}