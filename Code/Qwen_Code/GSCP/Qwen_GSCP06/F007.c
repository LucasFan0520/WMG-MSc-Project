// F007.c
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

typedef struct Act {
    int time;
    char* label;
    char* note;
    struct Act* next;
} Act;

Act* head = NULL;

int cmp_act(const void* a, const void* b) {
    Act* A = *(Act**)a;
    Act* B = *(Act**)b;
    if (A->time != B->time) return A->time - B->time;
    return strcmp(A->label, B->label);
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
            char* t_str = get_word(&p);
            char* label = get_word(&p);
            Act* n = malloc(sizeof(Act));
            n->time = atoi(t_str);
            n->label = strdup(label);
            n->note = strdup(p);
            n->next = NULL;
            if (!head) head = n;
            else {
                Act* tail = head;
                while (tail->next) tail = tail->next;
                tail->next = n;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* t_str = get_word(&p);
            char* label = get_word(&p);
            int t = atoi(t_str);
            Act* prev = NULL;
            for (Act* a = head; a; ) {
                Act* next = a->next;
                if (a->time == t && strcmp(a->label, label) == 0) {
                    if (prev) prev->next = next;
                    else head = next;
                    free(a->label);
                    free(a->note);
                    free(a);
                } else {
                    prev = a;
                }
                a = next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* label = get_word(&p);
            for (Act* a = head; a; a = a->next) {
                if (strcmp(a->label, label) == 0) {
                    printf("%d %s %s\n", a->time, a->label, a->note);
                    break;
                }
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            int count = 0;
            for (Act* a = head; a; a = a->next) count++;
            Act** arr = malloc(count * sizeof(Act*));
            Act* a = head;
            for (int i = 0; i < count; i++) {
                arr[i] = a;
                a = a->next;
            }
            qsort(arr, count, sizeof(Act*), cmp_act);
            head = NULL;
            Act* tail = NULL;
            for (int i = 0; i < count; i++) {
                arr[i]->next = NULL;
                if (!head) head = arr[i];
                else tail->next = arr[i];
                tail = arr[i];
            }
            free(arr);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            while (head) {
                Act* temp = head;
                head = head->next;
                free(temp->label);
                free(temp->note);
                free(temp);
            }
        }
    }
    return 0;
}