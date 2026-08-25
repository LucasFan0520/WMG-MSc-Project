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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *word;
    int count;
} Freq;

int main(int argc, char **argv) {
    Freq *freqs = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                if (strlen(line) > 0) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(freqs[i].word, line) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        freqs[found].count++;
                    } else {
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            freqs = realloc(freqs, cap * sizeof(Freq));
                        }
                        freqs[count].word = strdup(line);
                        freqs[count].count = 1;
                        count++;
                    }
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "WORD ", 5) == 0) {
            char *word = line + 5;
            while (*word == ' ') word++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, word) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                freqs[found].count++;
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    freqs = realloc(freqs, cap * sizeof(Freq));
                }
                freqs[count].word = strdup(word);
                freqs[count].count = 1;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *word = line + 7;
            while (*word == ' ') word++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, word) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(freqs[found].word);
                for (int i = found; i < count - 1; i++) {
                    freqs[i] = freqs[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *word = line + 6;
            while (*word == ' ') word++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(freqs[i].word, word) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%d\n", freqs[found].count);
            else printf("0\n");
        } else if (strcmp(line, "TOP") == 0) {
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    if (freqs[i].count < freqs[j].count) {
                        Freq tmp = freqs[i];
                        freqs[i] = freqs[j];
                        freqs[j] = tmp;
                    }
                }
            }
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", freqs[i].word, freqs[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(freqs[i].word);
    }
    free(freqs);
    return 0;
}
