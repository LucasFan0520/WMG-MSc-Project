// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int b_count;
} Record;

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
    Record *list = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *title = read_line(fp);
                if (!title) break;
                if (strcmp(title, "--") == 0) {
                    free(title);
                    continue;
                }
                list = realloc(list, (count + 1) * sizeof(Record));
                list[count].title = title;
                list[count].body = NULL;
                list[count].b_count = 0;
                while (1) {
                    char *body_line = read_line(fp);
                    if (!body_line) break;
                    if (strcmp(body_line, "--") == 0) {
                        free(body_line);
                        break;
                    }
                    int bc = list[count].b_count;
                    list[count].body = realloc(list[count].body, (bc + 1) * sizeof(char *));
                    list[count].body[bc] = body_line;
                    list[count].b_count++;
                }
                count++;
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
            if (strcmp(line, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", list[i].title);
                    for (int j = 0; j < list[i].b_count; j++) {
                        printf("%s\n", list[i].body[j]);
                    }
                    if (i < count - 1) printf("--\n");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *title = p1 + 1;
        if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    printf("%s\n", list[i].title);
                    for (int j = 0; j < list[i].b_count; j++) {
                        printf("%s\n", list[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].title);
                for (int j = 0; j < list[found].b_count; j++) {
                    free(list[found].body[j]);
                }
                free(list[found].body);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Record));
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            list = realloc(list, (count + 1) * sizeof(Record));
            list[count].title = strdup(title);
            list[count].body = NULL;
            list[count].b_count = 0;
            while (1) {
                char *body_line = read_line(stdin);
                if (!body_line) break;
                if (strcmp(body_line, "--") == 0) {
                    free(body_line);
                    break;
                }
                int bc = list[count].b_count;
                list[count].body = realloc(list[count].body, (bc + 1) * sizeof(char *));
                list[count].body[bc] = body_line;
                list[count].b_count++;
            }
            count++;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].title);
        for (int j = 0; j < list[i].b_count; j++) {
            free(list[i].body[j]);
        }
        free(list[i].body);
    }
    free(list);
    return 0;
}