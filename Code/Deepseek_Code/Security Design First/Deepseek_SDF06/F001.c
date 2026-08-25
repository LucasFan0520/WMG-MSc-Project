// F001.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

static Paragraph *paragraphs = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void add_paragraph(const char *id, const char *text) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Paragraph *new = realloc(paragraphs, capacity * sizeof(Paragraph));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        paragraphs = new;
    }
    paragraphs[count].id = strdup(id);
    paragraphs[count].text = strdup(text);
    if (!paragraphs[count].id || !paragraphs[count].text) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static ssize_t find_paragraph(const char *id) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(paragraphs[i].id, id) == 0)
            return (ssize_t)i;
    return -1;
}

static void append_text(size_t idx, const char *text) {
    size_t old_len = strlen(paragraphs[idx].text);
    size_t new_len = old_len + strlen(text) + 1;
    char *new_text = malloc(new_len);
    if (!new_text) { fprintf(stderr, "Memory error\n"); exit(1); }
    snprintf(new_text, new_len, "%s%s", paragraphs[idx].text, text);
    free(paragraphs[idx].text);
    paragraphs[idx].text = new_text;
}

static void replace_text(size_t idx, const char *text) {
    char *new_text = strdup(text);
    if (!new_text) { fprintf(stderr, "Memory error\n"); exit(1); }
    free(paragraphs[idx].text);
    paragraphs[idx].text = new_text;
}

static void delete_paragraph(size_t idx) {
    free(paragraphs[idx].id);
    free(paragraphs[idx].text);
    for (size_t i = idx; i < count - 1; i++)
        paragraphs[i] = paragraphs[i + 1];
    count--;
}

static void show_paragraph(size_t idx) {
    for (char *p = paragraphs[idx].text; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void list_paragraphs(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s\n", paragraphs[i].id);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(paragraphs[i].id);
        free(paragraphs[i].text);
    }
    free(paragraphs);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], id[256], text_buf[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text_buf) == 2) {
                if (find_paragraph(id) == -1)
                    add_paragraph(id, text_buf);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text_buf) == 2) {
                ssize_t idx = find_paragraph(id);
                if (idx != -1)
                    append_text(idx, text_buf);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text_buf) == 2) {
                ssize_t idx = find_paragraph(id);
                if (idx != -1)
                    replace_text(idx, text_buf);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) {
                ssize_t idx = find_paragraph(id);
                if (idx != -1)
                    delete_paragraph(idx);
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) {
                ssize_t idx = find_paragraph(id);
                if (idx != -1)
                    show_paragraph(idx);
                else
                    printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }

    free(line);
    free_all();
    return 0;
}