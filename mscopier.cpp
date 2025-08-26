#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <filesystem>

using std::filesystem::path;

void *CopyFile() {
    path destinationFolder = "destination_dir";
    pthread_exit(NULL); 
}

int main(int argc, char *argv[]) {
    int numOfThreads = 0;

    int threadReturnValue;

    // Store the Thread Identifiers
    pthread_t threads[numOfThreads];

    for (int k = 0; k < numOfThreads; k++) {
        threadReturnValue = pthread_create(&threads[k], NULL, CopyFile, (void *)t);
    }

    // Terminate the Calling Thread
    pthread_exit(NULL);
}