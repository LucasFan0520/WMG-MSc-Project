// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *stop;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int stop_count = 0;

Stop *get_stop(int index) {
    Stop *s = head;
    int i = 0;
    while (s) {
        if (i == index) return s;
        s = s->next;
        i++;
    }
    return NULL;
}

void add_stop(const char *stop, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->stop = malloc(strlen(stop)+1);
    strcpy(s->stop, stop);
    s->note = malloc(strlen(note)+1);
    strcpy(s->note, note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    stop_count++;
}

void insert_stop(int index, const char *stop, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *new = malloc(sizeof(Stop));
    new->stop = malloc(strlen(stop)+1);
    strcpy(new->stop, stop);
    new->note = malloc(strlen(note)+1);
    strcpy(new->note, note);
    if (index == 0) {
        new->next = head;
        head = new;
        if (!tail) tail = new;
    } else if (index == stop_count) {
        new->next = NULL;
        if (tail) tail->next = new;
        tail = new;
    } else {
        Stop *prev = get_stop(index-1);
        new->next = prev->next;
        prev->next = new;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    Stop *prev = NULL;
    Stop *s = head;
    int i = 0;
    while (s) {
        if (i == from) break;
        prev = s;
        s = s->next;
        i++;
    }
    if (!s) return;
    if (prev) prev->next = s->next;
    else head = s->next;
    if (s == tail) tail = prev;
    s->next = NULL;
    if (to == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *before = get_stop(to-1);
        s->next = before->next;
        before->next = s;
        if (s->next == NULL) tail = s;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
        if (!head) tail = NULL;
    } else {
        Stop *prev = get_stop(index-1);
        to_del = prev->next;
        prev->next = to_del->next;
        if (index == stop_count-1) tail = prev;
    }
    free(to_del->stop);
    free(to_del->note);
    free(to_del);
    stop_count--;
}

void find_stop(const char *stop) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->stop, stop) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->stop, s->note);
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = head;
    while (s) {
        Stop *next = s->next;
        free(s->stop);
        free(s->note);
        free(s);
        s = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = strtok(rest, " ");
            if (!stop) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_stop(stop, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = strtok(rest, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            char *stop = strtok(NULL, " ");
            if (!stop) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            insert_stop(idx, stop, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_str = strtok(rest, " ");
            char *to_str = strtok(NULL, " ");
            if (from_str && to_str) move_stop(atoi(from_str), atoi(to_str));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_str = strtok(rest, " ");
            if (idx_str) remove_stop(atoi(idx_str));
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = strtok(rest, " ");
            if (stop) find_stop(stop);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}