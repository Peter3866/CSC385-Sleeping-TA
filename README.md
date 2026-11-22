# CSC 385 — Sleeping Teaching Assistant Project

**Students:** Ralph Petit and Kyle Fish  
**Course:** CSC 385 — Operating Systems  
**Language:** C  
**Environment:** Ubuntu Linux

## 1. Project Overview

This project simulates the Sleeping Teaching Assistant problem using C and POSIX threads. One teaching assistant helps five students, with three chairs available for waiting. The program uses mutex locks and semaphores to coordinate the threads and prevent race conditions.

## 2. Code Explanation

### Header Files
The program includes `stdio.h`, `stdlib.h`, `pthread.h`, `semaphore.h`, and `unistd.h`. These provide functions for printing messages, creating threads, synchronization, and delays.

### Constants
```c
#define STUDENTS 5
#define CHAIRS 3
#define ROUNDS 3
```
These define five students, three waiting chairs, and three attempts per student.

### Mutex and Semaphores
```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t waiting_students;
sem_t called[STUDENTS], finished[STUDENTS];
```
The mutex protects shared variables. The semaphores notify the TA when students arrive, tell students when they are called, and signal when assistance is complete.

### Waiting Queue
```c
int queue[CHAIRS];
int front = 0, rear = 0, count = 0;
```
The program uses a circular FIFO queue to manage students waiting for help. Students are served in the order they enter the queue.

### Teaching Assistant Function — `ta()`
The TA waits for students using `sem_wait()`. When a student arrives, the TA selects that student, provides assistance, and signals completion. The TA exits when all student threads finish and no requests remain.

### Student Function — `student()`
Each student attempts to receive help three times. If the TA is available, the student requests help directly. Otherwise, the student waits in an available chair. If all chairs are occupied, the student skips that attempt.

### Main Function — `main()`
The main function initializes the semaphores, creates one TA thread and five student threads, waits for the threads to finish, and releases the synchronization resources.

## 3. Compile and Run

Compile the program in Ubuntu:

```bash
gcc -pthread sleeping_ta.c -o sleeping_ta
```

Run the program:

```bash
./sleeping_ta
```

## 4. Operating Systems Concepts

This project demonstrates:

- **Multithreading:** Multiple students execute concurrently.
- **Mutex locks:** Protect shared data from race conditions.
- **Semaphores:** Coordinate communication between threads.
- **FIFO queue:** Manages students waiting for assistance.
- **Thread synchronization:** Ensures the TA helps one student at a time.

## 5. Conclusion

This project helped us understand how operating systems manage concurrent threads and shared resources. By using mutex locks, semaphores, and a circular queue, we implemented a simulation of the Sleeping Teaching Assistant problem.

---

**Students:** Ralph Petit and Kyle Fish  
**Course:** CSC 385 — Operating Systems
