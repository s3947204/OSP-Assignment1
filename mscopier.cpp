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

bool doneReading = false;
// constraint on lines
const int MAX_QUEUE_SIZE = 20;
std::queue<std::string> sharedQueue;

struct Data {
    std::string source;     
    std::string destination;
    int id;
    std::string fileName; 
};

std::ifstream inputFile;
std::ofstream outputFile;

void *readerThread(void *arg) {
    Data *data = (Data *)arg;
    if (!data) pthread_exit((void*)1);

    std::string line;
    while (true) {
        if (!std::getline(inputFile, line)) {
            doneReading = true;
            break;
        }
        if ((int)sharedQueue.size() < MAX_QUEUE_SIZE) {
            sharedQueue.push(line);
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void *writerThread(void *arg) {
    Data *data = (Data *)arg;
    if (!data) pthread_exit((void*)1);

    while (true) {
        if (!sharedQueue.empty()) {
            std::string line = sharedQueue.front();
            sharedQueue.pop();
            outputFile << line << "\n";
        } else if (doneReading) {
            break;
        }
    }
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

    int numThreads = atoi(numStr.c_str());
    if (numThreads < 2 || numThreads > 10) {
        std::cerr << "Error: num_threads must be between 2 and 10.\n";
        // uncomment once sync is implemented************************************
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
    std::cout << "Copying finished. Note that locks and CVs not yet implemented\n";
    return 0;
}