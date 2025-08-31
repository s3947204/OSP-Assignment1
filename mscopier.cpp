
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <stdexcept>

struct Data {
  std::string source;
  std::string destination;
  int id;
};



void *CopyFile() {
 
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  // Check the number of arguments
  if (argc != 4) {
    std::cerr << "Missing arguments!" << std::endl;
    return 1;
  }

  // Store command line arguments
  std::string num = argv[1];
  std::string source = argv[2];
  std::string destination = argv[3];

  int numOfThreads = 0;
  // Convert n to an integer and check if it's valid
  try {
    numOfThreads = std::stoi(num);
    if (numOfThreads < 2 || numOfThreads > 10) {
      std::cerr << "n must be between 2 and 10." << std::endl;
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << "n must be an integer between 2 and 10." << std::endl;
    return 1;
  }
  // Check if source directory and destination directory exist
  struct stat sourcesb, destinationsb;
  if (stat(source.c_str(), &sourcesb) != 0 || !S_ISDIR(sourcesb.st_mode)) {
    std::cerr << "The source directory is not valid!" << std::endl;
    return 1;
  } else if (stat(destination.c_str(), &destinationsb) != 0 ||
             !S_ISDIR(destinationsb.st_mode)) {
    std::cerr << "The destination directory is not valid!" << std::endl;
    return 1;
  }

  int threadReturnValue;

  // Use a struct to store messages
  Data sharedData;
  sharedData.source = source;
  sharedData.destination = destination;

  // Store the Thread Identifiers
  pthread_t threads[numOfThreads];

  for (int k = 0; k < numOfThreads; k++) {
    // Increment id by 1 to keep track
    sharedData.id = k + 1;
    int threadReturnValue = pthread_create(&threads[k], NULL, CopyFile, &sharedData);
    if (threadReturnValue != 0) {
      std::cerr << "Creating thread failed!" << std::endl;
      return 1;
    }
  }

  // Terminate the Calling Thread
  for (int i = 0; i < numOfThreads; i++) {
    pthread_join(threads[i], NULL);
  }
  return 0;
}