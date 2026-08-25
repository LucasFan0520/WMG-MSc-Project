// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *head = NULL, *tail = NULL;

Word *find_word(const char *text) {
    Word *w = head;
    while (w) {
        if (strcmp(w->text, text) == 0) return w;
        w = w->next;
    }
    return NULL;
}

void add_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        Word *new = malloc(sizeof(Word));
        new->text = malloc(strlen(text) + 1);
        strcpy(new->text, text);
        new->count = 1;
        new->next = NULL;
        if (tail) {
            tail->next = new;
            tail = new;
        } else {
            head = tail = new;
        }
    }
}

void remove_word(const char *text) {
    Word *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_word(const char *text) {
    Word *w = find_word(text);
    printf("%d\n", w ? w->count : 0);
}

void top_words(void) {
    int count = 0;
    Word *w = head;
    while (w) { count++; w = w->next; }
    if (count == 0) return;
    Word **arr = malloc(count * sizeof(Word *));
    int i = 0;
    w = head;
    while (w) { arr[i++] = w; w = w->next; }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->count < arr[j]->count) {
                Word *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        add_word(line);
    }
    free(line);
    fclose(f);
}

void free_all(void) {
    Word *w = head;
    while (w) {
        Word *next = w->next;
        free(w->text);
        free(w);
        w = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "WORD") == 0) {
            char *text = strtok(NULL, " ");
            if (text) add_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *text = strtok(NULL, " ");
            if (text) remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *text = strtok(NULL, " ");
            if (text) count_word(text);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_words();
        }
    }
    free(line);
    free_all();
    return 0;
}