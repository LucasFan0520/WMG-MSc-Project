// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *title;
    char **body;
    int body_count;
} Record20;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Record20 *catalog = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *title_line = read_line(fp);
        if (!title_line) break;
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            Record20 *new_cat = realloc(catalog, capacity * sizeof(Record20));
            if (new_cat) catalog = new_cat;
        }
        if (count < capacity) {
            catalog[count].title = title_line;
            catalog[count].body = NULL;
            catalog[count].body_count = 0;
            int b_cap = 0;
            while (1) {
                char *b_line = read_line(fp);
                if (!b_line) break;
                if (strcmp(b_line, "--") == 0) {
                    free(b_line);
                    break;
                }
                if (catalog[count].body_count >= b_cap) {
                    b_cap = b_cap == 0 ? 4 : b_cap * 2;
                    char **new_b = realloc(catalog[count].body, b_cap * sizeof(char *));
                    if (new_b) catalog[count].body = new_b;
                }
                catalog[count].body[catalog[count].body_count] = b_line;
                catalog[count].body_count++;
            }
            count++;
        } else {
            free(title_line);
        }
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title_start = line + 5;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title_start) == 0) {
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title_start = line + 7;
            while (*title_start == ' ') title_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title_start) == 0) {
                    free(catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        free(catalog[i].body[j]);
                    }
                    free(catalog[i].body);
                    for (int j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title_start = line + 4;
            while (*title_start == ' ') title_start++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record20 *new_cat = realloc(catalog, capacity * sizeof(Record20));
                if (new_cat) catalog = new_cat;
            }
            if (count < capacity) {
                catalog[count].title = strdup(title_start);
                catalog[count].body = NULL;
                catalog[count].body_count = 0;
                int b_cap = 0;
                while (1) {
                    char *b_line = read_line(stdin);
                    if (!b_line) break;
                    if (strcmp(b_line, "--") == 0) {
                        free(b_line);
                        break;
                    }
                    if (catalog[count].body_count >= b_cap) {
                        b_cap = b_cap == 0 ? 4 : b_cap * 2;
                        char **new_b = realloc(catalog[count].body, b_cap * sizeof(char *));
                        if (new_b) catalog[count].body = new_b;
                    }
                    catalog[count].body[catalog[count].body_count] = b_line;
                    catalog[count].body_count++;
                }
                count++;
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].body_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
                if (i < count - 1) printf("--\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].body_count; j++) {
            free(catalog[i].body[j]);
        }
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}
