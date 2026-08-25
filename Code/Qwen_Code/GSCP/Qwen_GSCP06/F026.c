// F026.c
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

typedef struct Note {
    char* cat;
    char* text;
    struct Note* next;
} Note;

Note* head = NULL;

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char* cat = get_word(&p);
            if (cat) {
                Note* n = malloc(sizeof(Note));
                n->cat = strdup(cat);
                n->text = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* cat = get_word(&p);
            Note* prev = NULL;
            for (Note* n = head; n; ) {
                Note* next = n->next;
                if (strcmp(n->cat, cat) == 0) {
                    if (prev) prev->next = next;
                    else head = next;
                    free(n->cat);
                    free(n->text);
                    free(n);
                } else {
                    prev = n;
                }
                n = next;
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char* cat = get_word(&p);
            Note* prev = NULL;
            for (Note* n = head; n; ) {
                Note* next = n->next;
                if (strcmp(n->cat, cat) != 0) {
                    if (prev) prev->next = next;
                    else head = next;
                    free(n->cat);
                    free(n->text);
                    free(n);
                } else {
                    prev = n;
                }
                n = next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* word = get_word(&p);
            for (Note* n = head; n; n = n->next) {
                if (strstr(n->text, word)) {
                    printf("%s\n", n->text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Note* n = head; n; n = n->next) {
                printf("%s\n", n->text);
            }
        }
    }
    return 0;
}