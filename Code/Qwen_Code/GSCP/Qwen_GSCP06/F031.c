// F031.c
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
    char* name;
    char* note;
    struct Rec* next;
} Rec;

Rec* head = NULL;

Rec* find_rec(const char* name) {
    for (Rec* r = head; r; r = r->next)
        if (strcmp(r->name, name) == 0) return r;
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

        if (strcmp(cmd, "BATCH") == 0) {
            char* n_str = get_word(&p);
            int n = atoi(n_str);
            if (n <= 0) continue;
            
            char** names = malloc(n * sizeof(char*));
            char** notes = malloc(n * sizeof(char*));
            int valid = 1;
            
            for (int i = 0; i < n; i++) {
                if (!fgets(line, sizeof(line), stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = 0;
                char* lp = line;
                char* name = get_word(&lp);
                if (!name || strlen(lp) == 0) {
                    valid = 0;
                    break;
                }
                names[i] = strdup(name);
                notes[i] = strdup(lp);
            }
            
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (!find_rec(names[i])) {
                        Rec* r = malloc(sizeof(Rec));
                        r->name = names[i];
                        r->note = notes[i];
                        r->next = head;
                        head = r;
                    } else {
                        free(names[i]);
                        free(notes[i]);
                    }
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (names[i]) free(names[i]);
                    if (notes[i]) free(notes[i]);
                }
            }
            free(names);
            free(notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Rec* prev = NULL;
            for (Rec* r = head; r; prev = r, r = r->next) {
                if (strcmp(r->name, name) == 0) {
                    if (prev) prev->next = r->next;
                    else head = r->next;
                    free(r->name);
                    free(r->note);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* name = get_word(&p);
            Rec* r = find_rec(name);
            if (r) printf("%s %s\n", r->name, r->note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec* r = head; r; r = r->next) {
                printf("%s %s\n", r->name, r->note);
            }
        }
    }
    return 0;
}