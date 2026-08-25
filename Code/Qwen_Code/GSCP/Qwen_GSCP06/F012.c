// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Person {
    char* name;
    char* note;
    struct Person* next;
} Person;

Person* head = NULL;
Person* tail = NULL;

Person* find_person(const char* name) {
    for (Person* p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }
    return NULL;
}

void add_person(const char* name, const char* note) {
    Person* n = malloc(sizeof(Person));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = NULL;
    if (!head) head = tail = n;
    else {
        tail->next = n;
        tail = n;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "JOIN") == 0) {
            char* name = get_word(&p);
            add_person(name, p);
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head) {
                printf("%s\n", head->name);
                Person* temp = head;
                head = head->next;
                if (!head) tail = NULL;
                free(temp->name);
                free(temp->note);
                free(temp);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char* name = get_word(&p);
            Person* prev = NULL;
            for (Person* per = head; per; prev = per, per = per->next) {
                if (strcmp(per->name, name) == 0) {
                    if (prev) prev->next = per->next;
                    else head = per->next;
                    if (tail == per) tail = prev;
                    free(per->name);
                    free(per->note);
                    free(per);
                    break;
                }
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char* name = get_word(&p);
            add_person(name, p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char* name = get_word(&p);
            Person* per = find_person(name);
            if (per) printf("%s\n", per->note);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (Person* per = head; per; per = per->next) {
                printf("%s\n", per->name);
            }
        }
    }
    return 0;
}