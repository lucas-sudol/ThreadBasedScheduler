#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h> 
#include <pthread.h>
#include <string.h>

#define MAX_STR 50
#define MAX_THREADS 50

int main(int argc, char **argv) {
    FILE * stream = NULL;

    // Process arguments
    if(argc == 2) { // File and no flag
        printf("")
    }
    else if (argc == 3){ // File and flag
        if (strcmp(strlwr(argv[2]), "true")) {
            stream = stdout;
        }
    }

    else {
        printf("Error! \n To run program: ./A1 fileName flag(true/false) \n");
        return 1;
    }

    return 0;
}
/*
//Global file pointer
FILE* writeFile = NULL;

// counter shared by all threads
int spellingErrors = 0;
int misspelled_ArrayCounter = 0;
int * misspelledWordCounter = NULL;
char ** misspelledWordArray = NULL;

int completeCounter = 0;

// mutex used to protect the counter and fileWrite
pthread_mutex_t misspelled_Mutex;
pthread_mutex_t fileWrite_Mutex;
pthread_mutex_t complete_Mutex;

//Print program menu
void usage()
{
	fprintf(stderr, "1. Start a new spellchecking task\n");
	fprintf(stderr, "2. Exit\n");
}

void addToMisspelled(char *textWord) {
     pthread_mutex_lock(&misspelled_Mutex);
     //Add to total number of spelling errors
     spellingErrors++;

    //Check for empty array
     if(misspelled_ArrayCounter == 0) {
        misspelled_ArrayCounter++;
        misspelledWordCounter = (int *) calloc(sizeof(int), misspelled_ArrayCounter);
        misspelledWordArray = (char**) malloc(sizeof(char*) * misspelled_ArrayCounter);

        misspelledWordCounter[0] = 1;
        misspelledWordArray[0] = strdup(textWord);
     }

     else {
        //Check first if exists in array
        int foundWord = 1;
        for(int i = 0; i < misspelled_ArrayCounter; i++) {
            if(!strcmp(misspelledWordArray[i], textWord)) {
                foundWord = 0;
                misspelledWordCounter[i]++;
            }
        }

        //If word is not found, realloc size of array and add
        if(foundWord) {
            misspelled_ArrayCounter++;
            misspelledWordCounter = (int *) realloc(misspelledWordCounter, sizeof(int)* misspelled_ArrayCounter);
            misspelledWordArray = (char**) realloc(misspelledWordArray, sizeof(char*) * misspelled_ArrayCounter);

            misspelledWordCounter[misspelled_ArrayCounter-1] = 1;
            misspelledWordArray[misspelled_ArrayCounter-1] = strdup(textWord);
        }
     }

     pthread_mutex_unlock(&misspelled_Mutex);
}

void addToLocalMisspelled(char *textWord, char*** misspelledArray, int ** countArray, int * arrayCounter) {
    //Check for empty array
    if(*arrayCounter == 0) {
        (*arrayCounter)= *arrayCounter + 1;
        (*countArray) = (int *) calloc(sizeof(int), *arrayCounter);
        (*misspelledArray) = (char**) malloc(sizeof(char*) * (*arrayCounter));

        *countArray [0] = 1;
        (*misspelledArray)[0] = strdup(textWord);
     }

     else {
        //Check first if exists in array
        int foundWord = 1;
        for(int i = 0; i < (*arrayCounter); i++) {
            if(!strcmp((*misspelledArray)[i], textWord)) {
                foundWord = 0;
                (*countArray)[i]++;
            }
        }

        //if word is not found, realloc array size and add word
        if(foundWord) {
            *arrayCounter = *arrayCounter + 1;
            (*countArray)  = (int *) realloc((*countArray), sizeof(int)* (*arrayCounter));
            (*misspelledArray) = (char**) realloc((*misspelledArray), sizeof(char*) * (*arrayCounter));

            (*countArray)[(*arrayCounter)-1] = 1;
            (*misspelledArray)[(*arrayCounter)-1] = strdup(textWord);
        }
     }
}

//Spellchecker
void* spellchecker(void * arg) {
    //Initialize variables and structures
    char *textWord = NULL;
    int foundWord;
    int misSpelled = 0;

    struct WordExtractorHolder* arguments = (struct WordExtractorHolder*) arg;

    struct WordExtractor* dictExtractor = arguments->dictionary;
    struct WordExtractor* wordExtractor = arguments->text;
    char* dictionary = arguments->dictionaryFile;
    char* fileName = arguments->textFile;

    //Create local storage of mispelled words
    char** misspelledArray = NULL;
    int * countArray = NULL;
    int arrayCounter = 0;

    //Traverse words in text file
    while (weHasMoreWords(wordExtractor)) {
        //Fetch next word
        textWord = weGetNextWord(wordExtractor);
        foundWord = 1;

        //Traverse words in dictionary file, and when found matching word exit
        while(foundWord && weHasMoreWords(dictExtractor)) {
            if(!strcmp(textWord, weGetNextWord(dictExtractor))) {
                foundWord = 0;
            }
        }

        //Only if word is not found, add to misspelled arrays
        if(foundWord) {
            misSpelled++;
            //Add to global array,
            addToMisspelled(textWord);
            //Add to local array
            addToLocalMisspelled(textWord, &misspelledArray, &countArray, &arrayCounter);
        }

        //Re-create dictionary to start parsing words again
        weDeleteExtractor(dictExtractor);
        dictExtractor = weCreateExtractor(dictionary, MAX_STR);
    }

    //3 mispelled words
    if(arrayCounter > 2)
    {
        //Traverse arrays and find top 3
        int firstMissSpelled = 0;
        int firstIndex = -1;
        int seccondMissSpelled = 0;
        int seccondIndex = -1;
        int thirdMissSpelled = 0;
        int thirdIndex = -1;

        for(int i = 0; i < arrayCounter; i++) {
            if(countArray[i] > firstMissSpelled) {
                thirdMissSpelled = seccondMissSpelled;
                thirdIndex = seccondIndex;


                seccondMissSpelled = firstMissSpelled;
                seccondIndex = firstIndex;

                firstMissSpelled = countArray[i];
                firstIndex = i;
            }

            else if(countArray[i] > seccondMissSpelled && countArray[i] != firstMissSpelled) {
                thirdMissSpelled = seccondMissSpelled;
                thirdIndex = seccondIndex;
                seccondMissSpelled = countArray[i];
                seccondIndex = i;
            }
            else if(countArray[i] > thirdMissSpelled && countArray[i] != seccondMissSpelled && countArray[i] != firstMissSpelled) {
                thirdMissSpelled = countArray[i];
                thirdIndex = i;
            }
        }
        //Write to file the top 3 mispelled words
        pthread_mutex_lock(&fileWrite_Mutex);
        fprintf(writeFile, "%s %d %s %s %s\n", fileName, misSpelled, misspelledArray[firstIndex], misspelledArray[seccondIndex], misspelledArray[thirdIndex]);
        pthread_mutex_unlock(&fileWrite_Mutex);
    }
    //2 Misspelled words
    else if (arrayCounter == 2) {
        //Check order of 2 mispelled words, then write to file
        if(countArray[0] > countArray[1]) {
            pthread_mutex_lock(&fileWrite_Mutex);
            fprintf(writeFile, "%s %d %s %s\n", fileName, misSpelled, misspelledArray[0], misspelledArray[1]);
            pthread_mutex_unlock(&fileWrite_Mutex);
        }
        else {
            pthread_mutex_lock(&fileWrite_Mutex);
            fprintf(writeFile, "%s %d %s %s\n", fileName, misSpelled, misspelledArray[1], misspelledArray[0]);
            pthread_mutex_unlock(&fileWrite_Mutex);
        }
    }
    //1 misspelled word, write results to file
    else if (arrayCounter == 1) {
        pthread_mutex_lock(&fileWrite_Mutex);
        fprintf(writeFile, "%s %d %s\n", fileName, misSpelled, misspelledArray[0]);
        pthread_mutex_unlock(&fileWrite_Mutex);
    }
    //No misspelled words
    else {
        pthread_mutex_lock(&fileWrite_Mutex);
        fprintf(writeFile, "%s %d\n", fileName, misSpelled);
        pthread_mutex_unlock(&fileWrite_Mutex);
    }

    // Close the file when we are done
    free(arguments);
    free(dictionary);
    free(fileName);
    weDeleteExtractor(dictExtractor);
    weDeleteExtractor(wordExtractor);

    //Free local arrays
    for(int i = 0; i< arrayCounter; i++) {
                free(misspelledArray[i]);
    }
    free(misspelledArray);
    free(countArray);

    //Complete Thread Counter
    pthread_mutex_lock(&complete_Mutex);
    completeCounter++;
    pthread_mutex_unlock(&complete_Mutex);

    fprintf(stderr, "Complete\n");

    return NULL;
}



int main(int argc, char **argv) {
    //Initialize variables
    char menuOption;
    char continueTask;
    char dictionary[MAX_STR];
    char textFile[MAX_STR];
    pthread_t tid[MAX_THREADS];
    int threads = 0;

    //Initialize filePointer
    writeFile = fopen("lsudol_A2.out", "w");
    FILE * summaryOutput = stderr;

    //Initialize Mutex
    pthread_mutex_init(&misspelled_Mutex, NULL);
    pthread_mutex_init(&fileWrite_Mutex, NULL);
    pthread_mutex_init(&complete_Mutex, NULL);
    
    //process arguments
    if(argc > 1) {
        for(int i = 1; i < argc; i++) {
            if(!strcmp(argv[i], "-l")) {
                summaryOutput = writeFile;
            }
        }
    }
    //Menu while loop
    do {
        //Menu
        usage();

        //Menu choice
        fprintf(stderr, "Enter choice: ");
        scanf(" %c", &menuOption);
        fprintf(stderr, "\n");

        //Create thread
        if(menuOption == '1') {
            //Variables for parser
            struct WordExtractor *dictExtractor = NULL;
            struct WordExtractor *wordExtractor = NULL;
            int exit = 1;

            //Enter dictionary File Name
            fprintf(stderr, "Enter dictionary file name: ");
            scanf("%s", dictionary);
            dictExtractor = weCreateExtractor(dictionary, MAX_STR);

            //Catch incorrect filename
            if (dictExtractor == NULL) {
		        fprintf(stderr, "Failed opening filename for '%s'\n", dictionary);
                exit = 0;
            }
            
            //Enter input text file name
            if(exit)
            {
                fprintf(stderr, "Enter input text file name: ");
                scanf("%s", textFile);
                wordExtractor = weCreateExtractor(textFile, MAX_STR);

                //Catch incorrect filename
                if (wordExtractor == NULL) {
                    fprintf(stderr, "Failed opening filename for '%s'\n", textFile);
                    exit = 0;
                }
            }

            //Check for correct filenames, then continue
            if(exit){
                fprintf(stderr, "Continue with task creation? [y/n]: ");
                scanf(" %c", &continueTask); 

                //Process thread creation
                if(continueTask == 'y') {
                    pthread_attr_t attr;
                    struct WordExtractorHolder* arguments = weCreateExtractorHolder(dictExtractor, wordExtractor, strdup(dictionary), strdup(textFile));

                    //Thread creation
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                    pthread_create(&tid[threads++], &attr, spellchecker, arguments);
                    pthread_attr_destroy(&attr); 
                }
                
                //Cancelled file creation
                else {
                    fprintf(stderr, "\nTask creation cancelled!\n\n");
                }
            }
            //Incorrect file names error message
            else {
                fprintf(stderr, "\nTask creation cancelled! Please provide correct file name!\n\n");
            }
        }
        
        //Menu option 2 - Exit
        else if (menuOption == '2') {
            //Print overall summary

            //All threads completed
            pthread_mutex_lock(&misspelled_Mutex);
            pthread_mutex_lock(&fileWrite_Mutex);
            pthread_mutex_lock(&complete_Mutex);

            if(threads == completeCounter) {
                fprintf(summaryOutput, "All threads have finished running!\n");
            }
            //Incomplete Threads
            else {
                fprintf(summaryOutput, "!!There are still threads running on exit!! (Summary may be incorrect as thread was still in progress)\n");
                fprintf(summaryOutput, "Number of threads still running: %3d\n", threads - completeCounter);
            }

            fprintf(summaryOutput, "Number of files processed: %3d\n", completeCounter);
            fprintf(summaryOutput, "Number of spelling errors: %3d\n", spellingErrors);


            //Calculate Misspellings
            //3 mispelled words
            if(misspelled_ArrayCounter > 2)
            {
                int firstMissSpelled = 0;
                int firstIndex = -1;
                int seccondMissSpelled = 0;
                int seccondIndex = -1;
                int thirdMissSpelled = 0;
                int thirdIndex = -1;

                for(int i = 0; i < misspelled_ArrayCounter; i++) {
                    if(misspelledWordCounter[i] > firstMissSpelled) {
                        thirdMissSpelled = seccondMissSpelled;
                        thirdIndex = seccondIndex;

                        seccondMissSpelled = firstMissSpelled;
                        seccondIndex = firstIndex;

                        firstMissSpelled = misspelledWordCounter[i];
                        firstIndex = i;
                    }

                    else if(misspelledWordCounter[i] > seccondMissSpelled && misspelledWordCounter[i] != firstMissSpelled) {
                        thirdMissSpelled = seccondMissSpelled;
                        thirdIndex = seccondIndex;
                        seccondMissSpelled = misspelledWordCounter[i];
                        seccondIndex = i;
                    }
                    else if(misspelledWordCounter[i] > thirdMissSpelled && misspelledWordCounter[i] != seccondMissSpelled && misspelledWordCounter[i] != firstMissSpelled) {
                        thirdMissSpelled = misspelledWordCounter[i];
                        thirdIndex = i;
                    }
                }

                fprintf(summaryOutput,"Tree most common misspellings: %s (%d times), %s (%d times), %s (%d times)\n", misspelledWordArray[firstIndex], misspelledWordCounter[firstIndex], misspelledWordArray[seccondIndex], misspelledWordCounter[seccondIndex], misspelledWordArray[thirdIndex], misspelledWordCounter[thirdIndex]);
            }
            //2 Misspelled words
            else if (misspelled_ArrayCounter == 2) {
                if(misspelledWordCounter[0] > misspelledWordCounter[1]) {
                    fprintf(summaryOutput,"Two most common misspellings: %s (%d times), %s (%d times)\n", misspelledWordArray[0], misspelledWordCounter[0], misspelledWordArray[1], misspelledWordCounter[1]);
                }
                else {
                    fprintf(summaryOutput,"Two most common misspellings: %s (%d times), %s (%d times)\n", misspelledWordArray[1], misspelledWordCounter[1], misspelledWordArray[0], misspelledWordCounter[0]);
                }
            }
            //1 misspelled word
            else if (misspelled_ArrayCounter == 1) {
                fprintf(summaryOutput,"Most common misspellings: %s (%d times)\n", misspelledWordArray[0], misspelledWordCounter[0]);
            }

            //Cleanup global memory and close file pointer
            for(int i = 0; i< misspelled_ArrayCounter; i++) {
                free(misspelledWordArray[i]);
            }
            free(misspelledWordArray);
            free(misspelledWordCounter);

            fclose(writeFile);

            pthread_mutex_unlock(&misspelled_Mutex);
            pthread_mutex_unlock(&fileWrite_Mutex);
            pthread_mutex_unlock(&complete_Mutex);
        }

        //Incorrect Input
        else  {
            fprintf(stderr, "Incorrect input! Please try again\n");
        }

    } while(menuOption != '2');
    return 0;
}
*/

