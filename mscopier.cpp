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
};

std::ifstream inputFile;
std::ofstream outputFile;

pthread_mutex_t mutex;

pthread_cond_t notFull;
pthread_cond_t notEmpty;

// Does Producer Consumer Problem
void *readerThread(void *arg) {

  Data *data = (Data *)arg;
  if (!data)
    pthread_exit((void *)1);

  std::string line;

  while (true) {
    // Acquire the Mutex Lock
    pthread_mutex_lock(&mutex);

    // Puts Reader to sleep if SharedQueue is Full and waits for the notFull
    // Signal
    while (sharedQueue.size() >= MAX_QUEUE_SIZE && !doneReading) {
      pthread_cond_wait(&notFull, &mutex);
    }

    if (!std::getline(inputFile, line)) {
      doneReading = true;

      // Release the Mutex Lock when done
      pthread_cond_signal(&notEmpty);
      pthread_mutex_unlock(&mutex);
      break;
    }

    sharedQueue.push(line);

    // Signal to Consumer that data is able to be consumed
    pthread_cond_signal(&notEmpty);
    pthread_mutex_unlock(&mutex);
  }

  pthread_exit(NULL);

  return NULL;
}

void *writerThread(void *arg) {

  Data *data = (Data *)arg;
  if (!data)
    pthread_exit((void *)1);

  while (true) {
    // Acquire the Mutex Lock
    pthread_mutex_lock(&mutex);

    // Puts Writer to sleep if SharedQueue is Empty and waits for the notFull
    // Signal
    while (sharedQueue.empty() && !doneReading) {
      pthread_cond_wait(&notEmpty, &mutex);
    }

    if (!sharedQueue.empty()) {
      std::string line = sharedQueue.front();
      sharedQueue.pop();

      outputFile << line << "\n";
      // Signal to Producer that data is able to be produced
      pthread_cond_signal(&notFull);
      pthread_mutex_unlock(&mutex);
    } else if (doneReading) {
      // Release the Mutex Lock when done
      pthread_mutex_unlock(&mutex);
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
  std::string destFile = argv[3];

  // Initialize the Mutex Lock
  pthread_mutex_init(&mutex, NULL);

  int numThreads = atoi(numStr.c_str());
  if (numThreads < 2 || numThreads > 10) {
    std::cerr << "Error: num_threads must be between 2 and 10.\n";
    return 1;
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
  Data threadArgs;
  threadArgs.source = sourceFile;
  threadArgs.destination = destFile;

  // Initialise the condition variables
  pthread_cond_init(&notFull, NULL);
  pthread_cond_init(&notEmpty, NULL);

  // stores thread identifiers
  std::vector<pthread_t> readers(numThreads), writers(numThreads);

  for (int i = 0; i < numThreads; i++) {
    int rc = pthread_create(&readers[i], NULL, readerThread, &threadArgs);
    if (rc != 0) {
      std::cerr << "Error: failed to create reader thread " << (i + 1)
                << " (pthread_create returned " << rc << ")\n";
      return 1;
    }
  }
  for (int i = 0; i < numThreads; i++) {
    int rc = pthread_create(&writers[i], NULL, writerThread, &threadArgs);
    if (rc != 0) {
      std::cerr << "Error: failed to create writer thread " << (i + 1)
                << " (pthread_create returned " << rc << ")\n";
      return 1;
    }
  }

  for (int i = 0; i < numThreads; i++) {
    int rc = pthread_join(readers[i], NULL);
    if (rc != 0) {
      std::cerr << "Error: failed to join reader thread " << (i + 1)
                << " (pthread_join returned " << rc << ")\n";
      return 1;
    }
  }
  for (int i = 0; i < numThreads; i++) {
    int rc = pthread_join(writers[i], NULL);
    if (rc != 0) {
      std::cerr << "Error: failed to join writer thread " << (i + 1)
                << " (pthread_join returned " << rc << ")\n";
      return 1;
    }
  }
  inputFile.close();
  outputFile.close();
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&notFull);
  pthread_cond_destroy(&notEmpty);

  return 0;
}