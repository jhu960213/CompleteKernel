//
//  shell.c
//  Kernel_Process
//
//  Created by Jack Hu on 2019-02-22.
//  Copyright © 2019 Jack Hu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"



// to display shell prompt indefinitley until script hits a certain exit code to finish and end process
void displayShellPromptAndPassCommand() {
    
    // instantiating char arrays to display prompt and store user inputs
    char prompt[100] = {'$',' '};
    char userInput[100];
    int errorCode = 0; // 0 meaning default code, no error
    
    // display the shell prompt indefinitley until an error code is reached and the function breaks out of the infinite loop
    while(1) {
        printf("%s",prompt);
        memset(userInput,'\0',100);
        // To account for when running script like this ./mysh < testfile.txt
        // When reading from terminal the fgets will eventually reach null
        // Thus when it reaches null then reroute stdin to keyboard
        if (fgets(userInput, sizeof(userInput), stdin) != NULL) {
            errorCode = parse(userInput);
        if (errorCode == -1) {
                break;
            }
        } else {
            freopen("/dev/tty", "r", stdin);
        }
        fflush(stdin);
    }
}

// this function when called parses user inputs and stores individual strings in an array of pointers for the interpretor to utilize
int parse(char ui[]) {
    
    // instantiating indexes and char arrays and array pointers to store parsed user input's address of the strings
    char tmp[100];
    char *words[100];
    memset(words,'\0',100);
    //*words = (char*)malloc((100)*sizeof(char));
    int i,j;
    int index = 0;
    
    for(i = 0; ui[i] == ' ' && i < 100; i++); // skipping over white spaces
    while(ui[i] != '\0' && i<100) {
        for(j = 0; ui[i]!='\0' && ui[i]!=' ' && i < 100; i++, j++) { // copying over letters
            tmp[j] = ui[i];
            // to account for the carriage return
            if (ui[i] == '\r') {
                j--;
            } else if (ui[i] == '\n') {
                j--;
            }
        }
        tmp[j] = '\0';
        words[index] = strdup(tmp); // mallocs a specific amounts of space in memory converts chars into string and stores the string's pointer at k = 0 of words
        for(i = i; ui[i] == ' ' && i < 100; i++); // skipping over white spaces until the next user input string
        index++; // increment the words pointer array index
    }
    
    /*printf("%s\n", words[0]);
     printf("%s\n", words[1]);
     printf("%s\n", words[2]);*/
    return interpreter(words);
}

