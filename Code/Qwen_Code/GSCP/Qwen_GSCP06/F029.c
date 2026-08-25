// F029.c
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

typedef struct Rec {
    char* key;
    char* val;
    struct Rec* next;
} Rec;

Rec* head = NULL;

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char* key = get_word(&p);
            if (key) {
                Rec* n = malloc(sizeof(Rec));
                n->key = strdup(key);
                n->val = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            Rec* curr = head;
            while (curr) {
                Rec* prev = curr;
                Rec* next = curr->next;
                while (next) {
                    if (strcmp(next->key, curr->key) == 0) {
                        prev->next = next->next;
                        free(next->key);
                        free(next->val);
                        free(next);
                        next = prev->next;
                    } else {
                        prev = next;
                        next = next->next;
                    }
                }
                curr = curr->next;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* key = get_word(&p);
            Rec* prev = NULL;
            for (Rec* r = head; r; ) {
                Rec* next = r->next;
                if (strcmp(r->key, key) == 0) {
                    if (prev) prev->next = next;
                    else head = next;
                    free(r->key);
                    free(r->val);
                    free(r);
                } else {
                    prev = r;
                }
                r = next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* key = get_word(&p);
            for (Rec* r = head; r; r = r->next) {
                if (strcmp(r->key, key) == 0) {
                    printf("%s\n", r->val);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec* r = head; r; r = r->next) {
                printf("%s %s\n", r->key, r->val);
            }
        }
    }
    return 0;
}