# OSP-Project 1

## Multithreading & Synchonisation

**Authors:**
- Ritish Kandel
- Morgan Xiao
- Matthew Kristanto
- Qianyi Li

## Compilation Instructions (On Core Teaching Servers)
Log into one of the teaching servers (jupiter.csit.rmit.edu.au, saturn.csit.rmit.edu.au, or titan.csit.rmit.edu.au)
- via a 'UNIX' like OS:
```bash
ssh sXXXXXXX@jupiter.csit.rmit.edu.au   # or saturn/titan
```
- via windows (on a terminal emulator):

E.g. For Putty

enter:
```bash
sXXXXXXX@jupiter.csit.rmit.edu.au   # or saturn/titan
```

in "Hostname" or "IP address" and then login using your associated RMIT password.


## Compile using the Makefile:

```bash
make            # builds both mmcopier & mscopier
```
```bash
make mmcopier   # builds mmcopier only
```
```bash
make mscopier   # builds mscopier only
```
```bash
make clean      # removes binaries & object files
```


## Running the Programs
**mscopier**

Copies a single source file into a destination file using *n* number of reader/writer threads (between 2 and 10).

```bash
./mscopier <num_threads> <source_file> <destination_file>
```

E.g.
```bash
./mscopier 5 source.txt dest.txt
```

**mmcopier**

Copies multiple files from a source directory to a destination directory using *n* worker threads.

```bash
./mmcopier <num_threads> <source_directory> <destination_directory>
```

E.g.
```bash
./mmcopier 5 source_dir dest_dir
```

## Testing

```bash
./[name of test-cript].sh
```
- Executes mmcopier or mscopier for 2–10 threads
- Checks that destination files match the source files
- Uses valgrind to detect memory leaks


## Locks & Condition Variables
File: mscopier.cpp

pthread_mutex_t mutex; (line 26)

pthread_cond_t notFull, notEmpty; (lines 28, 29)


pthread_mutex_lock(&mutex); (lines 43, 79)

pthread_cond_wait(&notFull, &mutex); (line 47) 

pthread_cond_signal(&notEmpty); (lines 54, 62) 

pthread_mutex_unlock(&mutex); (lines 55, 63, 94, 97) 

pthread_cond_wait(&notEmpty, &mutex); (line 84)

pthread_cond_signal(&notFull); (line 93)
> Note: mmcopier.cpp doesn't require shared locks/CVs since each worker writes to distinct files. It only uses independent file ops, so no sync primitives are necessary.

## Team Work Log Table
| Student name | Student No. |Tasks Completed | Hours Spent | Date |
|-------|-------|-------|-------|-------|
| Ritish Kandel | s3947204      | Tested and wrote scripts for both tasks in the server      | 6      | 07/09/2025      |
| Morgan Xiao | S4095270      | Completed subtask 1 of task 2, created makefile, and readme       |  6     | 07/09/2025       |
| Matthew Kristanto | s4008599      | Completed subtask 2 and 3 of task 2, as well as skeleton for task 1      | 6      | 07/09/2025      |
| Qianyi Li | S4112793      | Completed task 1 and fixed deadlock issue for task 2      | 6     | 07/09/2025      | 