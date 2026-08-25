/* F001.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

static Paragraph *paragraphs = NULL;
static int pcount = 0;
static int pcap = 0;

static int find_para(const char *id) {
    for (int i = 0; i < pcount; i++) {
        if (strcmp(paragraphs[i].id, id) == 0)
            return i;
    }
    return -1;
}

static int ensure_cap(void) {
    if (pcount >= pcap) {
        int newcap = pcap == 0 ? 8 : pcap * 2;
        Paragraph *tmp = realloc(paragraphs, sizeof(Paragraph) * newcap);
        if (!tmp) return -1;
        paragraphs = tmp;
        pcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static void print_underscore(const char *s) {
    for (size_t i = 0; s[i]; i++) {
        putchar(s[i] == ' ' ? '_' : s[i]);
    }
    putchar('\n');
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "NEW") == 0) {
            char id[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", id, &off2) != 1) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            if (find_para(id) >= 0) continue;
            if (ensure_cap() != 0) continue;
            paragraphs[pcount].id = dup_str(id);
            paragraphs[pcount].text = dup_str(text);
            if (!paragraphs[pcount].id || !paragraphs[pcount].text) {
                free(paragraphs[pcount].id);
                free(paragraphs[pcount].text);
                continue;
            }
            pcount++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char id[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", id, &off2) != 1) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            int idx = find_para(id);
            if (idx < 0) continue;
            size_t oldlen = strlen(paragraphs[idx].text);
            size_t addlen = strlen(text);
            char *newtext = malloc(oldlen + 1 + addlen + 1);
            if (!newtext) continue;
            memcpy(newtext, paragraphs[idx].text, oldlen);
            newtext[oldlen] = ' ';
            memcpy(newtext + oldlen + 1, text, addlen + 1);
            free(paragraphs[idx].text);
            paragraphs[idx].text = newtext;
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char id[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", id, &off2) != 1) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            int idx = find_para(id);
            if (idx < 0) continue;
            char *newtext = dup_str(text);
            if (!newtext) continue;
            free(paragraphs[idx].text);
            paragraphs[idx].text = newtext;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, " %255s", id) != 1) continue;
            int idx = find_para(id);
            if (idx < 0) continue;
            free(paragraphs[idx].id);
            free(paragraphs[idx].text);
            for (int i = idx; i < pcount - 1; i++) {
                paragraphs[i] = paragraphs[i + 1];
            }
            pcount--;
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, " %255s", id) != 1) continue;
            int idx = find_para(id);
            if (idx < 0) {
                printf("NOT_FOUND\n");
            } else {
                print_underscore(paragraphs[idx].text);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < pcount; i++) {
                printf("%s\n", paragraphs[i].id);
            }
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(paragraphs[i].id);
        free(paragraphs[i].text);
    }
    free(paragraphs);
    return 0;
}