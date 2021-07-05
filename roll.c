#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define MAXLEN 1000

char* replace(char* line);
int find(char* str, regex_t* regex, int* match_start, int* match_end);
char* roll(char* dice);
char* concat(char* dest, char* src, size_t src_sz);
int count_digits(int n);
void memchk(void* ptr);
int getln(char* buf, int lim);
void putln(char* buf, int lim);

int 
main(int argc, char *argv[])
{
    int i;
    char* s = NULL;
    time_t t;
    char line[MAXLEN];

    srand((unsigned) time(&t));

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            s = replace(argv[i]);
            putln(s, strlen(s));
            free(s);
        }
    } else {
        while ((getln(line, MAXLEN)) > 0) {
            s = replace(line);
            putln(s, strlen(s));
            free(s);
        }
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
        memchk(dice);
        strncpy(dice, line + match_start, match_size);
        dice[match_size] = 0;

        result = roll(dice);
        if (result == NULL) {
            s[0] = 0;
            break;
        }

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
    memchk(dest);

    for (i = dest_sz, j = 0; j < src_sz; i++, j++) {
        dest[i] = src[j];
    }
    dest[i] = 0;

    return dest;
}

char*
roll(char* dice)
{
    char *result = NULL;
    char delim[] = "d";
    char *left, *right;
    int amount, type;
    int i, n = 0, digits;

    left = strtok(dice, delim);
    right = strtok(NULL, delim);

    amount = atoi(left);
    if (amount < 1) {
        return NULL; 
    }

    type = atoi(right);
    if (type < 1) {
        return NULL;
    }

    for (i = 0; i < amount; i++) {
        n += (rand() % type) + 1;
    }

    digits = count_digits(n);
    result = malloc(sizeof(char) * digits);
    memchk(result);

    sprintf(result, "%d", n);

    return result;
}

int
count_digits(int n)
{
    int count = 0;

    while (n != 0) {
        n /= 10;
        count++;
    }

    return count;
}

void 
memchk(void* ptr)
{
    if (ptr != NULL)
        return;

    perror("out of memory!");
    exit(EXIT_FAILURE);
}

int 
getln(char* buf, int lim) 
{
    int i = 0, c;

    while ((c = getchar()) != EOF && c != '\n') {
        if (i >= lim-1)
            continue;
        buf[i] = c;
        i++;
    }

    buf[i] = 0;

    return i;
}

void 
putln(char* buf, int lim)
{
    write(stdout, buf, sizeof(char) * lim);
    write(stdout, "\n", sizeof(char));
}
