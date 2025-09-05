#include <cctype>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <vector>

struct Data {
  std::string source;
  std::string destination;
  int id;
  std::string fileName;
};

void *CopyFile(void *arg) {
  Data *data = (Data *)arg;

  std::ostringstream oss;
  oss << data->id;
  std::string id_str = oss.str();

  std::string sourceFile =
      data->source + "/" + data->fileName + id_str + ".txt";
  std::string destinationFile =
      data->destination + "/" + data->fileName + id_str + ".txt";

  // Open sourceFile for reading
  std::ifstream Reader(sourceFile.c_str());
  if (!Reader.is_open()) {
    std::cerr << "Can't open source file " << sourceFile << std::endl;
    pthread_exit(NULL);
  }
  std::ofstream Writer(destinationFile.c_str());
  if (!Writer.is_open()) {
    std::cerr << "Can't open destination file " << destinationFile << std::endl;
    Reader.close();
    pthread_exit(NULL);
  }
  std::string text;
  while (std::getline(Reader, text)) {
    Writer << text << std::endl;
  }

  Reader.close();
  Writer.close();

  pthread_exit(NULL);
  return NULL;
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

  int numOfThreads;

  numOfThreads = atoi(num.c_str());
  if (numOfThreads < 2 || numOfThreads > 10) {
    std::cerr << "n must be between 2 and 10." << std::endl;
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

  // Get file name in the source directory, like "source1.txt"
  DIR *dir = opendir(source.c_str());
  struct dirent *file;

  std::string fullName;
  while ((file = readdir(dir)) != NULL) {
    fullName = file->d_name;
    if (fullName != "." && fullName != "..") {
      break;
    }
  }

  closedir(dir);

  // Remove .txt at the end
  std::string fileName = fullName;
  size_t dot = fileName.rfind(".txt");
  if (dot != std::string::npos) {
    fileName = fileName.substr(0, dot);
  }

  while (!fileName.empty() && isdigit(fileName[fileName.length() - 1])) {
    fileName.erase(fileName.length() - 1);
  }

  // Use a struct to store messages
  std::vector<Data> sharedData(numOfThreads);
  for (int i = 0; i < numOfThreads; i++) {
    sharedData[i].source = source;
    sharedData[i].destination = destination;
    sharedData[i].id = i + 1;
    sharedData[i].fileName = fileName;
  }
  int threadReturnValue;

  // Store the Thread Identifiers
  std::vector<pthread_t> threads(numOfThreads);

  for (int k = 0; k < numOfThreads; k++) {
    threadReturnValue =
        pthread_create(&threads[k], NULL, CopyFile, &sharedData[k]);
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