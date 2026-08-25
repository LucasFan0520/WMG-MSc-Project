// F034.c
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
    char* name;
    char* text;
    struct Note* next;
} Note;

Note* head = NULL;

Note* find_note(const char* name) {
    for (Note* n = head; n; n = n->next)
        if (strcmp(n->name, name) == 0) return n;
    return NULL;
}

int main(int argc, char* argv[]) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                FILE* f = fopen(argv[1], "r");
                if (f) {
                    Note* new_head = NULL;
                    int valid = 1;
                    char fline[MAX_LINE];
                    while (fgets(fline, sizeof(fline), f)) {
                        fline[strcspn(fline, "\n")] = 0;
                        char* fp = fline;
                        char* name = get_word(&fp);
                        if (!name || strlen(fp) == 0) {
                            valid = 0;
                            break;
                        }
                        Note* n = malloc(sizeof(Note));
                        n->name = strdup(name);
                        n->text = strdup(fp);
                        n->next = new_head;
                        new_head = n;
                    }
                    fclose(f);
                    if (valid) {
                        while (head) {
                            Note* temp = head;
                            head = head->next;
                            free(temp->name);
                            free(temp->text);
                            free(temp);
                        }
                        head = new_head;
                    } else {
                        while (new_head) {
                            Note* temp = new_head;
                            new_head = new_head->next;
                            free(temp->name);
                            free(temp->text);
                            free(temp);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char* name = get_word(&p);
            if (name) {
                Note* n = find_note(name);
                if (n) {
                    free(n->text);
                    n->text = strdup(p);
                } else {
                    Note* nn = malloc(sizeof(Note));
                    nn->name = strdup(name);
                    nn->text = strdup(p);
                    nn->next = head;
                    head = nn;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Note* prev = NULL;
            for (Note* n = head; n; prev = n, n = n->next) {
                if (strcmp(n->name, name) == 0) {
                    if (prev) prev->next = n->next;
                    else head = n->next;
                    free(n->name);
                    free(n->text);
                    free(n);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Note* n = head; n; n = n->next) {
                printf("%s %s\n", n->name, n->text);
            }
        }
    }
    return 0;
}