/* F026.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char *category;
    char *text;
} Note;

static Note *notes = NULL;
static int ncount = 0;
static int ncap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int contains_word(const char *text, const char *word) {
    size_t wlen = strlen(word);
    const char *p = text;
    while ((p = strstr(p, word)) != NULL) {
        if ((p == text || !isalnum((unsigned char)p[-1])) &&
            (!isalnum((unsigned char)p[wlen]) || p[wlen] == '\0')) {
            return 1;
        }
        p++;
    }
    return 0;
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

        if (strcmp(cmd, "ADD") == 0) {
            char cat[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", cat, &off2) != 1) continue;
            const char *text = line + offset + off2;
            while (*text == ' ') text++;
            if (ncount >= ncap) {
                ncap = ncap == 0 ? 8 : ncap * 2;
                notes = realloc(notes, sizeof(Note) * ncap);
            }
            notes[ncount].category = dup_str(cat);
            notes[ncount].text = dup_str(text);
            ncount++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256] = {0};
            if (sscanf(line + offset, "%255s", cat) != 1) continue;
            int i = 0;
            while (i < ncount) {
                if (strcmp(notes[i].category, cat) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j + 1];
                    ncount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256] = {0};
            if (sscanf(line + offset, "%255s", cat) != 1) continue;
            int i = 0;
            while (i < ncount) {
                if (strcmp(notes[i].category, cat) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (int j = i; j < ncount - 1; j++) notes[j] = notes[j + 1];
                    ncount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256] = {0};
            if (sscanf(line + offset, "%255s", word) != 1) continue;
            for (int i = 0; i < ncount; i++) {
                if (contains_word(notes[i].text, word)) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ncount; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    for (int i = 0; i < ncount; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}