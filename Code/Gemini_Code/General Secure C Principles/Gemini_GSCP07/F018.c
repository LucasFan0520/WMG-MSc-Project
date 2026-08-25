// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *word;
    int count;
} WordFreq;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    WordFreq *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *w = line;
                while (*w == ' ') w++;
                size_t l = strlen(w);
                while (l > 0 && w[l - 1] == ' ') {
                    w[l - 1] = '\0';
                    l--;
                }
                if (*w != '\0') {
                    int found = 0;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(list[i].word, w) == 0) {
                            list[i].count++;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            WordFreq *nl = realloc(list, capacity * sizeof(WordFreq));
                            if (!nl) {
                                free(line);
                                break;
                            }
                            list = nl;
                        }
                        list[count].word = strdup(w);
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
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "WORD") == 0) {
            while (*p == ' ') p++;
            char *w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    list[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    WordFreq *nl = realloc(list, capacity * sizeof(WordFreq));
                    if (!nl) break;
                    list = nl;
                }
                list[count].word = strdup(w);
                list[count].count = 1;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    free(list[i].word);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int cval = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].word, w) == 0) {
                    cval = list[i].count;
                    break;
                }
            }
            printf("%d\n", cval);
        } else if (strcmp(cmd, "TOP") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = i + 1; j < count; j++) {
                    if (list[i].count < list[j].count) {
                        WordFreq tmp = list[i];
                        list[i] = list[j];
                        list[j] = tmp;
                    }
                }
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", list[i].word, list[i].count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].word);
    }
    free(list);
    return 0;
}
