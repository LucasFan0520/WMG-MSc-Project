/* F008.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
    char *name;
    int score;
    char *note;
    struct student *next;
} Student;

Student *head = NULL;

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
        return;
    }
    Student *new = malloc(sizeof(Student));
    new->name = strdup(name);
    new->score = score;
    new->note = strdup(note);
    new->next = head;
    head = new;
}

void update_student(const char *name, int score, const char *note) {
    add_student(name, score, note); // same as add (overwrites)
}

void drop_student(const char *name) {
    Student *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name); free(s->note); free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average() {
    int total = 0, count = 0;
    Student *s = head;
    while (s) { total += s->score; count++; s = s->next; }
    printf("%d\n", count ? total / count : 0);
}

void list_students() {
    Student *s = head;
    while (s) {
        printf("%s %d ", s->name, s->score);
        for (char *c = s->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        s = s->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], note[1024];
        int score;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "AVERAGE") == 0) { average(); continue; }
        if (strcmp(cmd, "LIST") == 0) { list_students(); continue; }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &score) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1 + (score >= 0 ? snprintf(NULL,0,"%d",score) : 0) + 1;
            if (*rest == ' ') rest++;
            if (strcmp(cmd, "ADD") == 0) add_student(name, score, rest);
            else update_student(name, score, rest);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) drop_student(name);
        }
    }
    return 0;
}