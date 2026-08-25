// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *word;
    int count;
    struct Word *next;
} Word;

Word *head = NULL;

Word* find_word(const char *word) {
    Word *cur = head;
    while (cur) {
        if (strcmp(cur->word, word) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void increment_word(const char *word) {
    Word *w = find_word(word);
    if (w) {
        w->count++;
    } else {
        Word *new = malloc(sizeof(Word));
        new->word = strdup(word);
        new->count = 1;
        new->next = NULL;
        if (!head) head = new;
        else {
            Word *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = new;
        }
    }
}

void remove_word(const char *word) {
    Word *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->word, word) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->word);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

int get_count(const char *word) {
    Word *w = find_word(word);
    return w ? w->count : 0;
}

void top_words() {
    // simple bubble sort on list (swap data)
    if (!head || !head->next) {
        Word *cur = head;
        while (cur) {
            printf("%s %d\n", cur->word, cur->count);
            cur = cur->next;
        }
        return;
    }
    int swapped;
    Word *ptr1, *ptr2;
    do {
        swapped = 0;
        ptr1 = head;
        while (ptr1->next) {
            ptr2 = ptr1->next;
            if (ptr1->count < ptr2->count) {
                // swap data
                char *tmp_word = ptr1->word;
                int tmp_count = ptr1->count;
                ptr1->word = ptr2->word;
                ptr1->count = ptr2->count;
                ptr2->word = tmp_word;
                ptr2->count = tmp_count;
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
    } while (swapped);
    Word *cur = head;
    while (cur) {
        printf("%s %d\n", cur->word, cur->count);
        cur = cur->next;
    }
}

void free_all() {
    Word *cur = head;
    while (cur) {
        Word *tmp = cur;
        cur = cur->next;
        free(tmp->word);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char word[1024];
            while (fgets(word, sizeof(word), f)) {
                word[strcspn(word, "\n")] = 0;
                if (strlen(word) > 0) increment_word(word);
            }
            fclose(f);
        }
    }
    char cmdline[4096];
    while (fgets(cmdline, sizeof(cmdline), stdin)) {
        cmdline[strcspn(cmdline, "\n")] = 0;
        if (strcmp(cmdline, "END") == 0) break;
        char *cmd = strtok(cmdline, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *word = strtok(NULL, " ");
            if (!word) continue;
            increment_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *word = strtok(NULL, " ");
            if (word) remove_word(word);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *word = strtok(NULL, " ");
            if (!word) continue;
            printf("%d\n", get_count(word));
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free_all();
    return 0;
}