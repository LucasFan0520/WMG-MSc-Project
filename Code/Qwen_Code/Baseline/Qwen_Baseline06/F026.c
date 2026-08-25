// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *cat, *text; } Note;
Note *notes; int n_note;

int main() {
    char cmd[20], cat[100], text[1000], word[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%s", cat);
            fgets(text, 1000, stdin);
            while(text[0]==' ') memmove(text, text+1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            notes = realloc(notes, (n_note+1)*sizeof(Note));
            notes[n_note].cat = strdup(cat);
            notes[n_note].text = strdup(text);
            n_note++;
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", cat);
            for(int i=0; i<n_note; ) {
                if(!strcmp(notes[i].cat, cat)) {
                    free(notes[i].cat);
                    free(notes[i].text);
                    for(int k=i; k<n_note-1; k++) notes[k] = notes[k+1];
                    n_note--;
                } else i++;
            }
        } else if(!strcmp(cmd, "KEEP")) {
            scanf("%s", cat);
            for(int i=0; i<n_note; ) {
                if(strcmp(notes[i].cat, cat) != 0) {
                    free(notes[i].cat);
                    free(notes[i].text);
                    for(int k=i; k<n_note-1; k++) notes[k] = notes[k+1];
                    n_note--;
                } else i++;
            }
        } else if(!strcmp(cmd, "FIND")) {
            scanf("%s", word);
            for(int i=0; i<n_note; i++) {
                if(strstr(notes[i].text, word)) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if(!strcmp(cmd, "PRINT")) {
            for(int i=0; i<n_note; i++) {
                printf("%s %s\n", notes[i].cat, notes[i].text);
            }
        }
    }
    return 0;
}