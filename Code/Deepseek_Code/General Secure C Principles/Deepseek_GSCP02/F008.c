// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

Student *head = NULL;
Student *tail = NULL;

Student *find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = NULL;
    if (tail) {
        tail->next = s;
        tail = s;
    } else {
        head = tail = s;
    }
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL;
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            if (s == tail) tail = prev;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score(void) {
    int total = 0, n = 0;
    Student *s = head;
    while (s) {
        total += s->score;
        n++;
        s = s->next;
    }
    if (n == 0) { printf("0\n"); return; }
    printf("%d\n", total / n);
}

void list_students(void) {
    Student *s = head;
    while (s) {
        printf("%s %d ", s->name, s->score);
        for (char *p = s->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    while (head) {
        Student *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
    tail = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char name[256], note[1024];
            int score;
            if (sscanf(line, "%*s %255s %d %1023[^\n]", name, &score, note) == 3) {
                add_student(name, score, note);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256], note[1024];
            int score;
            if (sscanf(line, "%*s %255s %d %1023[^\n]", name, &score, note) == 3) {
                update_student(name, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                drop_student(name);
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }

    free(line);
    free_all();
    return 0;
}