#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define STUDENTS 5
#define CHAIRS 3
#define ROUNDS 3

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t waiting_students;
sem_t called[STUDENTS], finished[STUDENTS];

int queue[CHAIRS];
int front = 0, rear = 0, count = 0;
int ta_busy = 0, direct_student = -1, done = 0;

void *ta(void *arg) {
    (void)arg;

    while (1) {
        sem_wait(&waiting_students);
        pthread_mutex_lock(&lock);

        if (done && direct_student == -1 && count == 0) {
            pthread_mutex_unlock(&lock);
            break;
        }

        int id;

        if (direct_student != -1) {
            id = direct_student;
            direct_student = -1;
        } else {
            id = queue[front];
            front = (front + 1) % CHAIRS;
            count--;
        }

        ta_busy = 1;
        printf("TA calls student %d\n", id + 1);
        pthread_mutex_unlock(&lock);

        sem_post(&called[id]);

        usleep(100000);
        printf("TA finished helping student %d\n", id + 1);
        sem_post(&finished[id]);

        pthread_mutex_lock(&lock);
        if (count == 0 && direct_student == -1)
            ta_busy = 0;
        pthread_mutex_unlock(&lock);
    }

    printf("TA goes home.\n");
    return NULL;
}

void *student(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < ROUNDS; i++) {
        usleep((id + 1) * 25000);

        pthread_mutex_lock(&lock);

        if (!ta_busy && count == 0 &&
            direct_student == -1) {
            ta_busy = 1;
            direct_student = id;
            printf("Student %d wakes the TA\n", id + 1);
        }
        else if (count < CHAIRS) {
            queue[rear] = id;
            rear = (rear + 1) % CHAIRS;
            count++;
            printf("Student %d waits (%d/3 chairs)\n",
                   id + 1, count);
        }
        else {
            printf("Student %d: no chair, return later\n",
                   id + 1);
            pthread_mutex_unlock(&lock);
            continue;
        }

        pthread_mutex_unlock(&lock);

        sem_post(&waiting_students);
        sem_wait(&called[id]);
        sem_wait(&finished[id]);
    }

    return NULL;
}

int main(void) {
    pthread_t ta_thread, students[STUDENTS];
    int ids[STUDENTS];

    sem_init(&waiting_students, 0, 0);

    for (int i = 0; i < STUDENTS; i++) {
        sem_init(&called[i], 0, 0);
        sem_init(&finished[i], 0, 0);
   }

    pthread_create(&ta_thread, NULL, ta, NULL);

    for (int i = 0; i < STUDENTS; i++) {
        ids[i] = i;
        pthread_create(&students[i], NULL,
                       student, &ids[i]);
    }

    for (int i = 0; i < STUDENTS; i++)
        pthread_join(students[i], NULL);

    pthread_mutex_lock(&lock);
    done = 1;
    pthread_mutex_unlock(&lock);

    sem_post(&waiting_students);
    pthread_join(ta_thread, NULL);

    for (int i = 0; i < STUDENTS; i++) {
        sem_destroy(&called[i]);
        sem_destroy(&finished[i]);
    }

    sem_destroy(&waiting_students);
    pthread_mutex_destroy(&lock);

    return 0;
}
