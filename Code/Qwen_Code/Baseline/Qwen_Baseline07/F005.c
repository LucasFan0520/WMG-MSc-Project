// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
int count = 0;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

void print_under(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char cmd[20], text[1024];
    int idx;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d", &idx); read_rest(text);
            if (idx < 0 || idx > count) continue;
            Line *nl = malloc(sizeof(Line)); nl->text = strdup(text);
            if (idx == 0) { nl->next = head; head = nl; }
            else {
                Line *curr = head;
                for (int i = 0; i < idx - 1; i++) curr = curr->next;
                nl->next = curr->next; curr->next = nl;
            }
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            read_rest(text);
            Line *nl = malloc(sizeof(Line)); nl->text = strdup(text); nl->next = NULL;
            if (!head) head = nl;
            else {
                Line *curr = head;
                while (curr->next) curr = curr->next;
                curr->next = nl;
            }
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d", &idx);
            if (idx < 0 || idx >= count) continue;
            Line *prev = NULL, *curr = head;
            for (int i = 0; i < idx; i++) { prev = curr; curr = curr->next; }
            if (prev) prev->next = curr->next; else head = curr->next;
            free(curr->text); free(curr); count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            scanf("%d", &idx); read_rest(text);
            if (idx < 0 || idx >= count) continue;
            Line *curr = head;
            for (int i = 0; i < idx; i++) curr = curr->next;
            free(curr->text); curr->text = strdup(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Line *curr = head; curr; curr = curr->next) print_under(curr->text);
        }
    }
    return 0;
}