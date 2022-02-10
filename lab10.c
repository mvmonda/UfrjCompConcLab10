#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM_BUFFER 10

// Vetor e variáveis de input para linha de comando
int numLeitores, numEscritores, *vetor;

// Ponteiro para o a "posição atual do buffer"
int ponteiroBuffer = TAM_BUFFER;
sem_t mutex, escrita;


void le(int id) {
    sem_wait(&mutex);
    
    if(ponteiroBuffer < TAM_BUFFER) {
        int valor = vetor[ponteiroBuffer];
        printf("Leitor %d leu %d do buffer\n", id, vetor[ponteiroBuffer]);
        ponteiroBuffer++;

        sem_post(&mutex);

        sleep(1); // Simula fazendo algo com o valor consumido
        printf("Leitor %d processou valor %d do buffer\n", id, vetor[ponteiroBuffer]);

        sem_wait(&mutex);
    } else {
        sem_post(&escrita);
    }

    
    sem_post(&mutex);
}

void escreve(int id) {
    // espera buffer esvaziar
    while(ponteiroBuffer < TAM_BUFFER)
        sem_wait(&escrita);
    
    for(int i = 0; i < TAM_BUFFER; i++) {
        vetor[i] = rand();
    }
    ponteiroBuffer = 0;
    printf("Escritor %d escreveu no buffer\n", id);
    sem_post(&escrita);
}

void *produtor(void * arg) {
    while(1) {
        escreve((int) arg);
    }
    pthread_exit(NULL);
}

void *consumidor(void * arg) {
    while(1) {
        le((int) arg);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    if(argc < 2) {
        fprintf(stderr, "Digite: %s <numero de leitores> <numero de consumidores>\n", argv[0]);
        return 1;
    }

    int inputLeitores = atoi(argv[1]);
    int inputEscritores = atoi(argv[2]);

    vetor = malloc(sizeof(int) * TAM_BUFFER);

    pthread_t tid[inputLeitores + inputEscritores];

    sem_init(&mutex, 0, 1);
    sem_init(&escrita, 0, 1);

    for(int i=0; i<inputLeitores + inputEscritores; i++) {
        if(pthread_create(tid+i, NULL, i < inputLeitores ? consumidor : produtor, (void*) i)){
            fprintf(stderr, "Erro ao criar thread\n");
            return 1;
        }
    }

    sleep(10);
}