/* F008.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
    char *name;
    int score;
    char *note;
    struct student *next;
} student_t;

student_t *head = NULL;
student_t *tail = NULL;

student_t *find_student(const char *name) {
    student_t *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    student_t *s = malloc(sizeof(student_t));
    if (!s) return;
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = NULL;
    if (!s->name || !s->note) {
        free(s->name); free(s->note); free(s);
        return;
    }
    if (tail) tail->next = s;
    else head = s;
    tail = s;
}

void update_student(const char *name, int score, const char *note) {
    student_t *s = find_student(name);
    if (!s) return;
    s->score = score;
    char *new = strdup(note);
    if (!new) return;
    free(s->note);
    s->note = new;
}

void drop_student(const char *name) {
    student_t *prev = NULL, *s = head;
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
    student_t *s = head;
    while (s) { total += s->score; n++; s = s->next; }
    if (n == 0) { printf("0\n"); return; }
    printf("%d\n", total / n);
}

void list_students(void) {
    student_t *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

void free_all(void) {
    student_t *s = head;
    while (s) {
        student_t *nxt = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], name[256];
        int score;
        if (sscanf(line, "%9s %255s %d", cmd, name, &score) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %255s %d", name, &score) == 2) {
                    const char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
                    while (*note == ' ') note++;
                    add_student(name, score, note);
                }
            } else if (strcmp(cmd, "UPDATE") == 0) {
                if (sscanf(line, "%*s %255s %d", name, &score) == 2) {
                    const char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
                    while (*note == ' ') note++;
                    update_student(name, score, note);
                }
            } else if (strcmp(cmd, "DROP") == 0) {
                drop_student(name);
            } else if (strcmp(cmd, "AVERAGE") == 0) {
                average_score();
            } else if (strcmp(cmd, "LIST") == 0) {
                list_students();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}