// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name, *note;
    struct Person *next;
} Person;

Person *head = NULL, *tail = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], name[256], note[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            scanf("%s", name); read_rest(note);
            Person *p = malloc(sizeof(Person));
            p->name = strdup(name); p->note = strdup(note); p->next = NULL;
            if (!head) head = tail = p; else { tail->next = p; tail = p; }
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head) {
                printf("%s\n", head->name);
                Person *next = head->next;
                free(head->name); free(head->note); free(head);
                head = next;
                if (!head) tail = NULL;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            Person *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    if (!curr->next) tail = prev;
                    free(curr->name); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            for (Person *p = head; p; p = p->next)
                if (strcmp(p->name, name) == 0) { printf("%s\n", p->note); break; }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (Person *p = head; p; p = p->next) printf("%s %s\n", p->name, p->note);
        }
    }
    return 0;
}