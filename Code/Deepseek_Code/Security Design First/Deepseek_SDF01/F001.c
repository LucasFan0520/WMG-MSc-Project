// filename: F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

static Paragraph *paragraphs = NULL;
static size_t pcount = 0;
static size_t pcap = 0;

static void cleanup(void) {
    for (size_t i = 0; i < pcount; ++i) {
        free(paragraphs[i].id);
        free(paragraphs[i].text);
    }
    free(paragraphs);
}

static int find_paragraph(const char *id) {
    for (size_t i = 0; i < pcount; ++i)
        if (strcmp(paragraphs[i].id, id) == 0)
            return (int)i;
    return -1;
}

static void add_paragraph(const char *id, const char *text) {
    if (find_paragraph(id) != -1) return;
    if (pcount == pcap) {
        size_t newcap = pcap ? pcap * 2 : 4;
        Paragraph *np = realloc(paragraphs, newcap * sizeof(Paragraph));
        if (!np) return;
        paragraphs = np;
        pcap = newcap;
    }
    paragraphs[pcount].id = strdup(id);
    paragraphs[pcount].text = strdup(text);
    if (!paragraphs[pcount].id || !paragraphs[pcount].text) {
        free(paragraphs[pcount].id);
        free(paragraphs[pcount].text);
        return;
    }
    ++pcount;
}

static void append_paragraph(const char *id, const char *text) {
    int idx = find_paragraph(id);
    if (idx == -1) return;
    size_t newlen = strlen(paragraphs[idx].text) + strlen(text) + 1;
    char *newtext = malloc(newlen);
    if (!newtext) return;
    snprintf(newtext, newlen, "%s%s", paragraphs[idx].text, text);
    free(paragraphs[idx].text);
    paragraphs[idx].text = newtext;
}

static void replace_paragraph(const char *id, const char *text) {
    int idx = find_paragraph(id);
    if (idx == -1) return;
    char *newtext = strdup(text);
    if (!newtext) return;
    free(paragraphs[idx].text);
    paragraphs[idx].text = newtext;
}

static void delete_paragraph(const char *id) {
    int idx = find_paragraph(id);
    if (idx == -1) return;
    free(paragraphs[idx].id);
    free(paragraphs[idx].text);
    for (size_t i = idx; i + 1 < pcount; ++i)
        paragraphs[i] = paragraphs[i + 1];
    --pcount;
}

static void show_paragraph(const char *id) {
    int idx = find_paragraph(id);
    if (idx == -1) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *p = paragraphs[idx].text; *p; ++p)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void list_paragraphs(void) {
    for (size_t i = 0; i < pcount; ++i)
        printf("%s\n", paragraphs[i].id);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *id = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *text = p;
        if (strcmp(cmd, "NEW") == 0) {
            if (*id) add_paragraph(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (*id) append_paragraph(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (*id) replace_paragraph(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*id) delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (*id) show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free(line);
    return 0;
}