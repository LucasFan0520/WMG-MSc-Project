// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        Word *w = malloc(sizeof(Word));
        if (!w) continue;
        w->text = strdup(line);
        w->count = 1;
        w->next = head;
        head = w;
    }
    fclose(f);
}

void add_word(const char *text) {
    for (Word *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->text, text) == 0) {
            curr->count++;
            return;
        }
    }
    Word *w = malloc(sizeof(Word));
    if (!w) return;
    w->text = strdup(text);
    w->count = 1;
    w->next = head;
    head = w;
}

void remove_word(const char *text) {
    Word *curr = head;
    Word *prev = NULL;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            Word *next = curr->next;
            free(curr->text);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_word(const char *text) {
    for (Word *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->text, text) == 0) {
            printf("%d\n", curr->count);
            return;
        }
    }
    printf("0\n");
}

void top_words() {
    Word *sorted = NULL;
    for (Word *curr = head; curr; curr = curr->next) {
        Word *w = malloc(sizeof(Word));
        if (!w) continue;
        w->text = curr->text;
        w->count = curr->count;
        
        Word *s_curr = sorted;
        Word *s_prev = NULL;
        while (s_curr) {
            if (w->count > s_curr->count) break;
            s_prev = s_curr;
            s_curr = s_curr->next;
        }
        w->next = s_curr;
        if (s_prev) s_prev->next = w;
        else sorted = w;
    }
    
    for (Word *curr = sorted; curr; curr = curr->next) {
        printf("%s %d\n", curr->text, curr->count);
    }
    
    Word *curr = sorted;
    while (curr) {
        Word *next = curr->next;
        free(curr);
        curr = next;
    }
}

void free_all() {
    Word *curr = head;
    while (curr) {
        Word *next = curr->next;
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char text[4096];
        
        int n = sscanf(line, "%15s %[^\n]", cmd, text);
        if (strcmp(cmd, "WORD") == 0 && n == 2) {
            add_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_word(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}