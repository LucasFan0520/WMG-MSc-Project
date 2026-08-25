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
int count = 0;

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
    s->name = malloc(strlen(name) + 1);
    strcpy(s->name, name);
    s->score = score;
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = head;
    head = s;
    count++;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
}

void drop_student(const char *name) {
    Student *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name);
            free(s->note);
            free(s);
            count--;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score(void) {
    if (count == 0) { printf("0\n"); return; }
    int sum = 0;
    Student *s = head;
    while (s) { sum += s->score; s = s->next; }
    printf("%d\n", sum / count);
}

void list_students(void) {
    Student *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

void free_all(void) {
    Student *s = head;
    while (s) {
        Student *tmp = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        int score;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s %d", name, &score) == 2) {
                const char *n = rest + strlen(name) + 1;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                if (*n) add_student(name, score, n);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(rest, "%255s %d", name, &score) == 2) {
                const char *n = rest + strlen(name) + 1;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                if (*n) update_student(name, score, n);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(rest, "%255s", name) == 1) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}