#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

using std::filesystem::path;

void *CopyFile() {
  path destinationFolder = "destination_dir";
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  // Check the number of arguments
  if (argc != 4) {
    return 1;
  }

  // Store command line arguments
  std::string num = argv[1];
  std::string source = argv[2];
  std::string destination = argv[3];

  int numOfThreads = 0;
  // Convert n to an integer
  try {
    numOfThreads = std::stoi(num);
    if (numOfThreads < 2 || numOfThreads > 10) {
      std::cerr << "n must be between 2 and 10." << std::endl;
      return 1;
    }
  } catch (...) {
    std::cerr << "n must be an integer between 2 and 10." << std::endl;
    return 1;
  }

  int threadReturnValue;

  // Store the Thread Identifiers
  pthread_t threads[numOfThreads];

  for (int k = 0; k < numOfThreads; k++) {
    threadReturnValue = pthread_create(&threads[k], NULL, CopyFile, (void *)t);
  }

  // Terminate the Calling Thread
  pthread_exit(NULL);
}