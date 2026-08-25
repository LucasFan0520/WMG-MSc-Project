// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct student {
    char *name;
    int score;
    char *note;
    struct student *next;
} Student;

Student *head = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Student *find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
        if (!s->note) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    Student *new = malloc(sizeof(Student));
    if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->name = strdup(name);
    new->note = strdup(note);
    if (!new->name || !new->note) { fprintf(stderr, "Memory error\n"); exit(1); }
    new->score = score;
    new->next = head;
    head = new;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
    if (!s->note) { fprintf(stderr, "Memory error\n"); exit(1); }
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
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score(void) {
    Student *s = head;
    int sum = 0, count = 0;
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

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "AVERAGE") == 0) { average_score(); continue; }
        if (strcmp(cmd, "LIST") == 0) { list_students(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *score_str = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            int score = atoi(score_str);
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "ADD") == 0) add_student(name, score, rest);
            else update_student(name, score, rest);
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_student(rest);
        }
    }
    free_all();
    return 0;
}