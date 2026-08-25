// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char cat[100];
    char text[1000];
} Note;

Note notes[1000];
int nn = 0;

int main() {
    char cmd[20], cat[100], text[1000], word[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", cat);
            fgets(text, 1000, stdin);
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            strcpy(notes[nn].cat, cat);
            strcpy(notes[nn].text, text);
            nn++;
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", cat);
            for (int i = 0; i < nn; ) {
                if (!strcmp(notes[i].cat, cat)) {
                    for (int j = i; j < nn - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    nn--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "KEEP")) {
            scanf("%s", cat);
            for (int i = 0; i < nn; ) {
                if (strcmp(notes[i].cat, cat) != 0) {
                    for (int j = i; j < nn - 1; j++) {
                        notes[j] = notes[j + 1];
                    }
                    nn--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", word);
            for (int i = 0; i < nn; i++) {
                if (strstr(notes[i].text, word)) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nn; i++) {
                printf("%s %s\n", notes[i].cat, notes[i].text);
            }
        }
    }
    return 0;
}