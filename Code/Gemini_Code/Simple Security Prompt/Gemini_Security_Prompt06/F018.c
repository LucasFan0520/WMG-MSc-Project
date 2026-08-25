// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                if (strlen(line) > 0) {
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].word, line) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        list[found].count++;
                    } else {
                        list = realloc(list, (count + 1) * sizeof(WordFreq));
                        list[count].word = strdup(line);
                        list[count].count = 1;
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
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "TOP") == 0) {
                for (int i = 0; i < count - 1; i++) {
                    for (int j = 0; j < count - i - 1; j++) {
                        if (list[j].count < list[j + 1].count) {
                            WordFreq temp = list[j];
                            list[j] = list[j + 1];
                            list[j + 1] = temp;
                        }
                    }
                }
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", list[i].word, list[i].count);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *text = p1 + 1;
        if (strcmp(cmd, "WORD") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                list[found].count++;
            } else {
                list = realloc(list, (count + 1) * sizeof(WordFreq));
                list[count].word = strdup(text);
                list[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].word);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(WordFreq));
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].word, text) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%d\n", list[found].count);
            } else {
                printf("0\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}