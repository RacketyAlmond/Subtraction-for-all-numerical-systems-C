#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define SYSTEM 10 //(2/8/10/16)


int validateNumber(int c) {
    if (SYSTEM == 2) {
        if ((c != '0' && c != '1') && !isspace(c)) {
            return 0;
        } else {
            return 1;
        }
    } else if (SYSTEM == 10 || SYSTEM == 8) {
        if (!(c >= '0' && c <= '9') && !isspace(c)) {
            return 0;
        } else {
            return 1;
        }
    } else if (SYSTEM == 16) {
        if (!isxdigit(c) && !isspace(c)) {
            return 0;
        } else {
            return 1;
        }
    }
}

char *getLiner(FILE *infile, char** lines, int numLines) {
    char *line;
    int size = 0;
    int capacity = 1;
    int c;
    char *temporary;
    if ((temporary = malloc(sizeof(char) * capacity)) == NULL) {
        fprintf(stderr, "Memory allocation error");
        fclose(infile);
        exit(1);
    }
    line = temporary;//malloc check na line
    int wrongCount = 0;

    while ((c = fgetc(infile)) != '\n' && c != EOF) {

        if (validateNumber(c) == 0) {
            wrongCount++;
            goto labeler;
        }
        if (!isspace(c)) {
            line[size++] = c;
        } else if (size > 0) {
            break;
        }
        labeler:
        if (size >= capacity) {
            if (capacity == 1) {
                capacity = 10;
            } else if (capacity < INT_MAX / 2) {
                capacity *= 2;
            } else if (capacity < INT_MAX - 100) {
                capacity += 100;
            } else {
                fprintf(stderr, "Stack overflow");
                return NULL;
            }

            char *temp;
            if ((temp = realloc(line, sizeof(char) * capacity)) == NULL) {
                fprintf(stderr, "Memory allocation error");
                free(line);//!!!!
                fclose(infile);
                exit(1);
            }
            line = temp;
        }
    }
    if (size == 0 && c == EOF) {
        free(line);
        return NULL;
    }
    line[size] = '\0';
    if (wrongCount != 0) {
        fprintf(stderr, "Wrong input number");
        for(int i = 0; i < numLines; i++){
            free(lines[i]);
        }
        free(lines);
        free(line);
        fclose(infile);
        exit(1);
    } else {
        return line;
    }
}

int ToDecimal(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

void minusNumbers(FILE *infile, char *num_0, char *num_1, char **lines, int numLines) {
    char *result = NULL;
    long int length_0 = strlen(num_0);
    long int length_1 = strlen(num_1);

    while(ToDecimal(num_0[0]) == 0 && length_0 != 1) {
        if (ToDecimal(num_0[0]) == 0 && length_0 != 1) {
            for (int i = 0; i < length_0; i++) {
                num_0[i] = num_0[i + 1];
            }
            length_0--;
        }
    }
    while(ToDecimal(num_1[0]) == 0 && length_1 != 1) {
        if (ToDecimal(num_1[0]) == 0 && length_1 != 1) {
            for (int i = 0; i < length_1; i++) {
                num_1[i] = num_1[i + 1];
            }
            length_1--;
        }
    }

    long int max_length = (length_0 > length_1) ? length_0 : length_1;
    char *temporary;
    if ((temporary = (char *) realloc(result, sizeof(char) * (max_length))) == NULL) {
        fprintf(stderr, "Memory allocation error");
        for (int h = 2; h < numLines; h++) {
            free(lines[h]);
        }
        free(lines);
        free(num_0);
        free(num_1);
        free(result);
        fclose(infile);
        exit(1);
    }
    result = temporary;

    long int k = max_length - 1;
    long int p = length_1 - 1;
    long int g = length_0 - 1;
    long int carry = 0;
    for (int i = 0; i < max_length; i++) {
        result[i] = 0;
    }
    int done = 0;
    int negative = 0;
    int resultLength = 0;

    int firstOneBigger = 1;
    while (p >= 0 || g >= 0) {
        long int bit1 = (g >= 0) ? ToDecimal(num_0[g]) : 0;
        long int bit2 = (p >= 0) ? ToDecimal(num_1[p]) : 0;
        long int sum;

        if (done == 0) {
            long int biggest = (g >= p) ? g : p;
            if(biggest == 0){
                biggest = 1;
            }
            if (length_0 != length_1) {
                firstOneBigger = (length_0 > length_1) ? 1 : 0;
                done = 1;
                goto labeler;
            }
            for (int i = 0; i < biggest + 1; i++) {
                if (ToDecimal(num_0[i]) > ToDecimal(num_1[i])) {
                    firstOneBigger = 1;
                    break;
                } else if (ToDecimal(num_0[i]) < ToDecimal(num_1[i])) {
                    firstOneBigger = 0;
                    break;
                }
            }
            done = 1;
        }
        labeler:

        if (firstOneBigger) {
            if (bit1 >= (bit2 + carry)) {
                sum = bit1 - bit2 - carry;
                carry = 0;
            } else {
                sum = SYSTEM + bit1 - bit2 - carry;
                carry = 1;
            }
            result[k] = sum % SYSTEM;
            //printf("result[%ld] = %d", k, result[k]);
            g--;
            p--;
            k--;
        } else {
            negative = 1;
            if (bit2 >= (bit1 + carry)) {
                sum = bit2 - bit1 - carry;
                carry = 0;
            } else {
                sum = SYSTEM + bit2 - bit1 - carry;
                carry = 1;
            }
            result[k] = sum % SYSTEM;
            g--;
            p--;
            k--;
        }
    }
    int zeroCount = 0;
    for (int i = 0; i < max_length; i++) {
        if (result[i] == 0) {
            zeroCount++;
        } else {
            break;
        }
    }
    for (int i = zeroCount; i < max_length; i++) {
        resultLength++;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////
    if (numLines > 2) {
        int isFirst = 1;
        char *num;
        for (long int j = 2; j < numLines; j++) {
            num = lines[j];
            if (num == NULL) {
                fprintf(stderr, "Error reading number from the file\n");
                for (int h = 2; h < numLines; h++) {
                    free(lines[h]);
                }
                free(lines);
                free(num_0);
                free(num_1);
                free(result);
                fclose(infile);
                exit(1);
            }

            unsigned int lengthNum = strlen(num);
            long int lengthResult = resultLength;

            firstOneBigger = 1;
            for (int i = 0; i < max_length; i++) {
                if (lengthResult != lengthNum) {
                    firstOneBigger = (lengthResult > lengthNum) ? 1 : 0;
                    goto labeling;
                }
                if (result[zeroCount + i] > ToDecimal(num[i])) {
                    firstOneBigger = 1;
                    break;
                } else if (result[zeroCount + i] < ToDecimal(num[i])) {

                    firstOneBigger = 0;
                    break;
                }
            }
            labeling:

            max_length = (lengthResult > lengthNum) ? lengthResult : lengthNum;

            char *tmp;
            if ((tmp = (char *) realloc(result, (max_length) * sizeof(char))) == NULL) {
                fprintf(stderr, "Memory allocation error");
                for (int h = 2; h < numLines; h++) {
                    free(lines[h]);
                }

                free(lines);
                free(num_0);
                free(num_1);
                free(result);
                fclose(infile);
                exit(1);
            }
            result = tmp;

            carry = 0;
            p = lengthResult - 1;
            g = lengthNum - 1;
            k = max_length - 1;
            long int sum;

            while (p >= 0 || g >= 0) {
                int bit1 = (p >= 0) ? result[p + zeroCount] : 0;
                int bit2 = (g >= 0) ? ToDecimal(num[g]) : 0;
                if (negative && isFirst) {
                    sum = bit1 + bit2 + carry;

                    result[k] = sum % SYSTEM;
                    carry = sum / SYSTEM;

                    g--;
                    p--;
                    k--;
                } else if (firstOneBigger) {
                    if (bit1 >= (bit2 + carry)) {
                        sum = bit1 - bit2 - carry;
                        carry = 0;
                    } else {
                        sum = SYSTEM + bit1 - bit2 - carry;
                        carry = 1;
                    }
                    result[k] = sum % SYSTEM;

                    g--;
                    p--;
                    k--;
                } else {
                    //printf("WESZEDL");
                    isFirst = 0;
                    negative = 1;
                    if (bit2 >= (bit1 + carry)) {
                        sum = bit2 - bit1 - carry;
                        carry = 0;
                    } else {
                        sum = SYSTEM + bit2 - bit1 - carry;
                        carry = 1;
                    }
                    result[k] = sum % SYSTEM;
                    //printf("result[%ld] = %d", k, result[k]);

                    g--;
                    p--;
                    k--;
                }
            }
        }
        free(num);
    }
///////////////////////////////////////////////////////////////////////////////////
    zeroCount = 0;
    for (int i = 0; i < max_length - 1; i++) {
        if (result[i] == 0) {
            zeroCount++;
        } else {
            break;
        }
    }
    printf("The result: ");


    if (negative) {
        printf("-");
    }

    for (int i = zeroCount; i < max_length; i++) {
        printf("%X", result[i]);
    }

    free(result);
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Bad input.");
        return EXIT_FAILURE;
    }
    FILE *infile = fopen(argv[1], "r");
    if (infile == NULL) {
        printf("Bad allocation.");
        if (fclose(infile) != 0) {
            printf("Error closing the file");
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }


    char *line;
    int numLines = 0;
    char **lines = NULL;
    while ((line = getLiner(infile, lines, numLines)) != NULL) {
        char **temp;
        if ((temp = realloc(lines, (numLines + 1) * sizeof(*lines))) == NULL) {
            printf("lines allocation error");
            for (int i = 0; i < numLines; i++) {
                free(lines[i]);
            }
            free(lines);
            if (fclose(infile) != 0) {
                printf("Error closing the file");
                return EXIT_FAILURE;
            }
            return EXIT_FAILURE;
        }
        lines = temp;
        lines[numLines] = line;
        numLines++;
    }
    if (numLines < 2) {
        fprintf(stderr, "\nIncorrect input error\n");
        for (int h = 0; h < numLines; h++) {
            free(lines[h]);
        }
        free(lines);
        if (fclose(infile) != 0) {
            printf("Error closing the file");
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }
    char *num_0 = lines[0];

    char *num_1 = lines[1];


    minusNumbers(infile, num_0, num_1, lines, numLines);

    //printf("numLines = %d", numLines);
    for (int h = 2; h < numLines - 1; h++) {
        free(lines[h]);
    }

    free(lines);
    free(num_0);
    free(num_1);
    fclose(infile);

    return 0;
}