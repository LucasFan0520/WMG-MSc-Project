// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *word;
    int count;
} WordFreq;

WordFreq *items = NULL;
size_t count = 0;
size_t cap = 0;

void add_word(char *word) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(items[i].word, word) == 0) {
            items[i].count++;
            return;
        }
    }
    if (count >= cap) {
        cap = cap == 0 ? 16 : cap * 2;
        WordFreq *nb = realloc(items, cap * sizeof(WordFreq));
        if (nb) items = nb;
    }
    items[count].word = strdup(word);
    items[count].count = 1;
    count++;
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                size_t wl = strlen(line);
                if (wl > 0 && line[wl - 1] == '\r') line[wl - 1] = '\0';
                if (strlen(line) > 0) {
                    add_word(line);
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "TOP") == 0) {
            WordFreq *copy = malloc(count * sizeof(WordFreq));
            if (copy) {
                for (size_t i = 0; i < count; i++) copy[i] = items[i];
                for (size_t i = 0; i < count; i++) {
                    for (size_t j = i + 1; j < count; j++) {
                        int swap = 0;
                        if (copy[i].count < copy[j].count) {
                            swap = 1;
                        } else if (copy[i].count == copy[j].count) {
                            if (strcmp(copy[i].word, copy[j].word) > 0) {
                                swap = 1;
                            }
                        }
                        if (swap) {
                            WordFreq tmp = copy[i];
                            copy[i] = copy[j];
                            copy[j] = tmp;
                        }
                    }
                }
                for (size_t i = 0; i < count; i++) {
                    printf("%s %d\n", copy[i].word, copy[i].count);
                }
                free(copy);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "WORD") == 0) {
            char *word = p;
            add_word(word);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *word = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, word) == 0) {
                    free(items[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *word = p;
            int c = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].word, word) == 0) {
                    c = items[i].count;
                    break;
                }
            }
            printf("%d\n", c);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].word);
    }
    free(items);
    return 0;
}
