// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_NOTE 4096

typedef struct Student {
    char name[MAX_NAME];
    int score;
    char note[MAX_NOTE];
    struct Student *next;
} Student;

Student *head = NULL;
int student_count = 0;

Student *find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void free_all() {
    Student *s = head;
    while (s) {
        Student *tmp = s;
        s = s->next;
        free(tmp);
    }
    head = NULL;
    student_count = 0;
}

void handle_add(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int score;
    int n = sscanf(line, "ADD %63s %d %4095[^\n]", name, &score, note);
    if (n < 2) return;
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    strcpy(s->name, name);
    s->score = score;
    if (n == 3) strcpy(s->note, note);
    else s->note[0] = '\0';
    s->next = head;
    head = s;
    student_count++;
}

void handle_update(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int score;
    int n = sscanf(line, "UPDATE %63s %d %4095[^\n]", name, &score, note);
    if (n < 2) return;
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    if (n == 3) strcpy(s->note, note);
    else s->note[0] = '\0';
}

void handle_drop(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DROP %63s", name) != 1) return;
    Student *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s);
            student_count--;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void handle_average() {
    if (student_count == 0) {
        printf("0\n");
        return;
    }
    int sum = 0;
    Student *s = head;
    while (s) {
        sum += s->score;
        s = s->next;
    }
    printf("%d\n", sum / student_count);
}

void handle_list() {
    Student *s = head;
    while (s) {
        printf("%s %d", s->name, s->score);
        if (s->note[0]) printf(" %s", s->note);
        putchar('\n');
        s = s->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "UPDATE ", 7) == 0) handle_update(line);
        else if (strncmp(line, "DROP ", 5) == 0) handle_drop(line);
        else if (strcmp(line, "AVERAGE") == 0) handle_average();
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}