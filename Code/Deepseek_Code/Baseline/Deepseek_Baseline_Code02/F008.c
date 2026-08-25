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
    s->name = malloc(strlen(name) + 1);
    strcpy(s->name, name);
    s->score = score;
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
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
    Student *prev = NULL;
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            if (tail == s) tail = prev;
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
    int sum = 0, count = 0;
    Student *s = head;
    while (s) {
        sum += s->score;
        count++;
        s = s->next;
    }
    if (count == 0) printf("0\n");
    else printf("%d\n", sum / count);
}

void list_students(void) {
    Student *s = head;
    while (s) {
        printf("%s %d ", s->name, s->score);
        for (char *c = s->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    Student *s = head;
    while (s) {
        Student *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            int score;
            if (sscanf(p, "%d", &score) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_student(name, score, p);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            int score;
            if (sscanf(p, "%d", &score) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            update_student(name, score, p);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            drop_student(p);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}