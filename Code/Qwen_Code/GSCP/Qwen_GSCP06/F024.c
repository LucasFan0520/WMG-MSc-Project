// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

void print_spaces(const char* s) {
    for (const char* p = s; *p; p++) {
        putchar(*p == '_' ? ' ' : *p);
    }
}

typedef struct Auth {
    char* name;
    struct Auth* next;
} Auth;

typedef struct Book {
    char* title;
    int year;
    Auth* auths;
    struct Book* next;
} Book;

Book* head = NULL;

Book* find_book(const char* title) {
    for (Book* b = head; b; b = b->next)
        if (strcmp(b->title, title) == 0) return b;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "BOOK") == 0) {
            char* title = get_word(&p);
            int year = atoi(p);
            if (title && !find_book(title)) {
                Book* n = malloc(sizeof(Book));
                n->title = strdup(title);
                n->year = year;
                n->auths = NULL;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char* title = get_word(&p);
            char* name = get_word(&p);
            Book* b = find_book(title);
            if (b && name) {
                Auth* n = malloc(sizeof(Auth));
                n->name = strdup(name);
                n->next = b->auths;
                b->auths = n;
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char* title = get_word(&p);
            char* name = get_word(&p);
            Book* b = find_book(title);
            if (b) {
                Auth* prev = NULL;
                for (Auth* a = b->auths; a; prev = a, a = a->next) {
                    if (strcmp(a->name, name) == 0) {
                        if (prev) prev->next = a->next;
                        else b->auths = a->next;
                        free(a->name);
                        free(a);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* title = get_word(&p);
            Book* prev = NULL;
            for (Book* b = head; b; prev = b, b = b->next) {
                if (strcmp(b->title, title) == 0) {
                    if (prev) prev->next = b->next;
                    else head = b->next;
                    free(b->title);
                    while (b->auths) {
                        Auth* temp = b->auths;
                        b->auths = b->auths->next;
                        free(temp->name);
                        free(temp);
                    }
                    free(b);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* title = get_word(&p);
            Book* b = find_book(title);
            if (b) {
                print_spaces(b->title);
                printf(" %d\n", b->year);
                for (Auth* a = b->auths; a; a = a->next) {
                    print_spaces(a->name);
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Book* b = head; b; b = b->next) {
                print_spaces(b->title);
                printf(" %d\n", b->year);
            }
        }
    }
    return 0;
}