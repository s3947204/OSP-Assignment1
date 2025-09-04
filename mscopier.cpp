#include <cctype>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>

// may become redundant once sync is implemented
bool doneReading = false;
// constraint on lines
const int MAX_QUEUE_SIZE = 20;
// might need mutex for thuis
std::queue<std::string> sharedQueue;

struct Data {
    std::string source;     
    std::string destination;
    int id;
    std::string fileName; 
};

/*
 * needs mutex for these 2 (corrupted reads and overlapping writes)
 * CV for waiting (writers if q is empty | readers if q is full)
*/ 
std::ifstream inputFile;
std::ofstream outputFile;

pthread_mutex_t mutex;

// Does Producer Consumer Problem
void *readerThread(void *arg) {
    // Acquire the Mutex Lock
    pthread_mutex_lock(&mutex); 

    Data *data = (Data *)arg;
    if (!data) pthread_exit((void*)1);

    std::string line;
    // Busy Waiting
    while (true) {
        if (!std::getline(inputFile, line)) {
            doneReading = true;
            break;
        }
        if ((int)sharedQueue.size() < MAX_QUEUE_SIZE) {
            sharedQueue.push(line);
        }
    }

    // Release the Mutex Lock when done
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);

    return NULL;
}

void *writerThread(void *arg) {
    // Acquire the Mutex Lock
    pthread_mutex_lock(&mutex); 

    Data *data = (Data *)arg;
    if (!data) pthread_exit((void*)1);

    //Busy Waiting
    while (true) {
        if (!sharedQueue.empty()) {
            std::string line = sharedQueue.front();
            sharedQueue.pop();
            outputFile << line << "\n";
        } else if (doneReading) {
            break;
        }
    }

    // Release the Mutex Lock when done
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);

    return NULL;
}

int main(int argc, char *argv[]) {
    // Check the number of arguments
    if (argc != 4) {
        std::cerr << "Correct Structure: " << argv[0]
                  << " <num_threads> <source_file> <dest_file>\n";
        return 1;
    }

    // Store command line arguments
    std::string numStr = argv[1];
    std::string sourceFile = argv[2];
    std::string destFile   = argv[3];

    // Initialize the Mutex Lock
    pthread_mutex_init(&mutex, NULL);

    int numThreads = atoi(numStr.c_str());
    if (numThreads < 2 || numThreads > 10) {
        std::cerr << "Error: num_threads must be between 2 and 10.\n";
        // uncomment once sync is implemented | currently doesnt work for 2+ threads
        //return 1;
    }

    inputFile.open(sourceFile.c_str());
    if (!inputFile.is_open()) {
        std::cerr << "Error opening source file " << sourceFile << "\n";
        return 1;
    }

    outputFile.open(destFile.c_str());
    if (!outputFile.is_open()) {
        std::cerr << "Error opening destination file " << destFile << "\n";
        return 1;
    }

    // thread handling 
    std::vector<Data> threadArgs(2 * numThreads);
    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].source = sourceFile;
        threadArgs[i].destination = destFile;
        threadArgs[i].id = i + 1;
        // writer args uses same
        threadArgs[numThreads + i] = threadArgs[i];
    }

    // stores thread identifiers
    std::vector<pthread_t> readers(numThreads), writers(numThreads);


    for (int i = 0; i < numThreads; i++) {
        int rc = pthread_create(&readers[i], NULL, readerThread, &threadArgs[i]);
        if (rc != 0) {
            std::cerr << "Error: failed to create reader thread " << (i+1)
                      << " (pthread_create returned " << rc << ")\n";
            return 1;
        }
    }
    for (int i = 0; i < numThreads; i++) {
        int rc = pthread_create(&writers[i], NULL, writerThread,
                                &threadArgs[numThreads + i]);
        if (rc != 0) {
            std::cerr << "Error: failed to create writer thread " << (i+1)
                      << " (pthread_create returned " << rc << ")\n";
            return 1;
        }
    }

    for (int i = 0; i < numThreads; i++) {
        int rc = pthread_join(readers[i], NULL);
        if (rc != 0) {
            std::cerr << "Error: failed to join reader thread " << (i+1)
                      << " (pthread_join returned " << rc << ")\n";
            return 1;
        }
    }
    for (int i = 0; i < numThreads; i++) {
        int rc = pthread_join(writers[i], NULL);
        if (rc != 0) {
            std::cerr << "Error: failed to join writer thread " << (i+1)
                      << " (pthread_join returned " << rc << ")\n";
            return 1;
        }
    }

    inputFile.close();
    outputFile.close();

    // progress tracker | delete when finished
    std::cout << "Copying finished. Note that CVs and Memory Leaks Fixation not yet implemented\n";
    return 0;
}