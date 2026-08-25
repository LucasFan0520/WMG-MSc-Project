// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Word {
    char *text;
    int count;
    struct Word *next;
} Word;

Word *words = NULL;

Word *find_word(const char *text) {
    for (Word *w = words; w; w = w->next) {
        if (strcmp(w->text, text) == 0) return w;
    }
    return NULL;
}

void add_word(const char *text) {
    Word *w = find_word(text);
    if (w) {
        w->count++;
    } else {
        w = malloc(sizeof(Word));
        w->text = strdup(text);
        w->count = 1;
        w->next = words;
        words = w;
    }
}

void remove_word(const char *text) {
    Word *prev = NULL, *curr = words;
    while (curr) {
        if (strcmp(curr->text, text) == 0) {
            if (prev) prev->next = curr->next;
            else words = curr->next;
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line())) {
        if (strlen(line) > 0) add_word(line);
        free(line);
    }
    fclose(f);
}

int cmp_words(const void *a, const void *b) {
    Word *wa = *(Word **)a;
    Word *wb = *(Word **)b;
    return wb->count - wa->count;
}

void print_top() {
    int count = 0;
    for (Word *w = words; w; w = w->next) count++;
    if (count == 0) return;
    
    Word **arr = malloc(count * sizeof(Word *));
    Word *curr = words;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    qsort(arr, count, sizeof(Word *), cmp_words);
    
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", arr[i]->text, arr[i]->count);
    }
    free(arr);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, text[256] = {0};
        sscanf(line, "%15s %255s", cmd, text);
        
        if (strcmp(cmd, "WORD") == 0) {
            add_word(text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_word(text);
        } else if (strcmp(cmd, "COUNT") == 0) {
            Word *w = find_word(text);
            printf("%d\n", w ? w->count : 0);
        } else if (strcmp(cmd, "TOP") == 0) {
            print_top();
        }
        free(line);
    }
    
    Word *w = words;
    while (w) {
        Word *next = w->next;
        free(w->text);
        free(w);
        w = next;
    }
    return 0;
}