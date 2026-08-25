// filename: F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

static Student *head = NULL;

static void cleanup(void) {
    while (head) {
        Student *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

static Student *find_student(const char *name) {
    for (Student *s = head; s; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

static void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(name);
    s->note = strdup(note);
    if (!s->name || !s->note) {
        free(s->name);
        free(s->note);
        free(s);
        return;
    }
    s->score = score;
    s->next = head;
    head = s;
}

static void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    char *new_note = strdup(note);
    if (!new_note) return;
    free(s->note);
    s->note = new_note;
    s->score = score;
}

static void drop_student(const char *name) {
    Student *prev = NULL;
    for (Student *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
    }
}

static void average_score(void) {
    int sum = 0, cnt = 0;
    for (Student *s = head; s; s = s->next) {
        sum += s->score;
        ++cnt;
    }
    printf("%d\n", cnt ? sum / cnt : 0);
}

static void list_students(void) {
    for (Student *s = head; s; s = s->next)
        printf("%s %d %s\n", s->name, s->score, s->note);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        int score = atoi(arg2);
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1) add_student(arg1, score, arg3);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (*arg1) update_student(arg1, score, arg3);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (*arg1) drop_student(arg1);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free(line);
    return 0;
}