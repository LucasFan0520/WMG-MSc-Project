// F027.c
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

typedef struct Rem {
    int pri;
    char* title;
    char* note;
    struct Rem* next;
} Rem;

Rem* head = NULL;

Rem* find_rem(const char* title) {
    for (Rem* r = head; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

int cmp_rem(const void* a, const void* b) {
    Rem* A = *(Rem**)a;
    Rem* B = *(Rem**)b;
    return B->pri - A->pri;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char* pri_str = get_word(&p);
            char* title = get_word(&p);
            if (pri_str && title) {
                Rem* n = malloc(sizeof(Rem));
                n->pri = atoi(pri_str);
                n->title = strdup(title);
                n->note = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char* title = get_word(&p);
            char* delta_str = get_word(&p);
            Rem* r = find_rem(title);
            if (r && delta_str) {
                r->pri += atoi(delta_str);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char* title = get_word(&p);
            Rem* prev = NULL;
            for (Rem* r = head; r; prev = r, r = r->next) {
                if (strcmp(r->title, title) == 0) {
                    if (prev) prev->next = r->next;
                    else head = r->next;
                    free(r->title);
                    free(r->note);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char* n_str = get_word(&p);
            int n = atoi(n_str);
            int count = 0;
            for (Rem* r = head; r; r = r->next) count++;
            Rem** arr = malloc(count * sizeof(Rem*));
            Rem* r = head;
            for (int i = 0; i < count; i++) {
                arr[i] = r;
                r = r->next;
            }
            qsort(arr, count, sizeof(Rem*), cmp_rem);
            int limit = n < count ? n : count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", arr[i]->pri, arr[i]->title, arr[i]->note);
            }
            free(arr);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Rem* r = head; r; r = r->next) {
                printf("%d %s %s\n", r->pri, r->title, r->note);
            }
        }
    }
    return 0;
}