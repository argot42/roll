#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

/*char* searchReplace(char* line);
int roll(char* dice);
char* replace(char* dest, char* src, int start, int end, int rpl);*/

char* replace(char* line);
int find(char* str, regex_t* regex, int* match_start, int* match_end);
char* roll(char* dice);
char* concat(char* dest, char* src, size_t src_sz);

int 
main(int argc, char *argv[])
{
    int i, n;
    char* s = NULL;

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            s = replace(argv[i]);
            printf("%s\n", s);
            free(s);
        }
    } else {
        printf("stdin\n");
    }

    return 0;
}

char*
replace(char* line)
{
    regmatch_t pmatch[1];
    regex_t regex;
    char* s = NULL;
    char* dice = NULL;
    char* result = NULL;
    int err;
    int match, match_start, match_end, match_size;

    err = regcomp(&regex, "[0-9][0-9]*d[0-9][0-9]*", 0);
    if (err != 0) {
        fprintf(stderr, "error compiling regex\n");
        exit(EXIT_FAILURE);
    }

    for(;;) {
        match = find(line, &regex, &match_start, &match_end);
        match_size = match_end - match_start;

        if (!match) {
            s = concat(s, line, strlen(line));
            break;
        }

        dice = realloc(dice, sizeof(char) * match_size);
        strncpy(dice, line + match_start, match_size);
        dice[match_size] = 0;

        result = roll(dice);

        s = concat(s, line, match_start);
        s = concat(s, result, strlen(result));

        strcpy(line, line + match_end);
    }

    free(dice);
    free(result);
    regfree(&regex);

    return s;
}

int
find(char* str, regex_t* regex, int* match_start, int* match_end)
{
    regmatch_t pmatch[1];

    if (strlen(str) == 0 || (regexec(regex, str, 1, pmatch, 0)) > 0) {
        return 0;
    }
    
    *match_start = pmatch[0].rm_so;
    *match_end = pmatch[0].rm_eo;

    return 1; 
}

char*
concat(char* dest, char* src, size_t src_sz)
{
    size_t i, j;
    size_t dest_sz;

    if (src_sz == 0) {
        return dest;
    }
    
    if (dest == NULL) {
        dest_sz = 0;
    } else {
        dest_sz = strlen(dest);
    }

    dest = realloc(dest, sizeof(char) * (dest_sz + src_sz));

    for (i = dest_sz, j = 0; j < src_sz; i++, j++) {
        dest[i] = src[j];
    }

    return dest;
}

char*
roll(char* dice)
{
    char *result = NULL;
    char delim[] = "d";
    char *left, *right;

    left = strtok(dice, delim);
    right = strtok(NULL, delim);

    printf("%s - %s\n", left, right);

    result = malloc(sizeof(char) * 2);

    result[0] = '0';
    result[1] = 0;

    return result;
}
