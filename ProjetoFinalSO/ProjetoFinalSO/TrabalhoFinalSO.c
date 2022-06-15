#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//definir o tamanho dos vários buffers
#define tamIN 200
#define tamCirc 50
#define tamOUT 198

//declaração dos buffers
int bfIN[tamIN];
int bfCirc[tamCirc];
int bfOUT[tamOUT];

//Contador para apresentar quantas vezes as threads produziram
//e consumiram elementos
int countP1 = 0;
int countP2 = 0;
int countC = 0;

//declaração de mutex para bloquear as Threads
pthread_mutex_t mutexProd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCons = PTHREAD_MUTEX_INITIALIZER;

//condição utilizadas para verificar se as produtoras podem continuar a produzir
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
//condição utilizadas para verificar se a consumidora pode continuar a produzir
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

//posição do buffer IN partir do qual se realiza o calculo da media
int posbfIN;
//posição do buffer OUT onde se vai inserir o próximo elemento do buffer circular
int posbfOut;

//indica se o buffer circular esta pronto para que uma thread produtora possa la colocar valores
// e indica se o buffer circular esta pronto para que a thread conasumidora possa consumir os seus valores
int ready; // variavel que vai funcionar para dizer que a thread está pronta a consumir ou não


void* producer(void*), * consumer(void*);


int main() {

    //ciclo para colocar os valores o buffer IN
    for (int i = 0, j = 1; i < tamIN; i++, j++) {
        bfIN[i] = j;
    }

    //declaração das threads
    pthread_t PM_T1, PM_T2, CM_T;

    //inicializção da Threads
    pthread_create(&PM_T1, NULL, producer, &countP1);
    pthread_create(&PM_T2, NULL, producer, &countP2);
    pthread_create(&CM_T, NULL, consumer, &countC);

    //Espera que as threads terminem de correr
    pthread_join(PM_T1, NULL);
    pthread_join(PM_T2, NULL);
    pthread_join(CM_T, NULL);

    //ciclo para imprimir na consola o buffer OUT
    printf("Buffer OUT : ");
    for (int i = 0; i < tamOUT; i++) {
        printf("%d |", bfOUT[i]);
    }
    printf("\n\n");

    //ciclo para imprimir na consola o buffer Circular
    printf("Buffer CIRC : ");
    for (int i = 0; i < tamCirc; i++) {
        printf("%d |", bfCirc[i]);
    }
    printf("\n\n");

    //imprimir na consola o que cada threads consumiu/produziu
    printf("\nProdutora 1: %d ", countP1);
    printf("\nProdutora 2:  %d ", countP2);
    printf("\nConsumidora: %d \n", countC);

    return 0;
}


void* producer(void* count) {

    while (1) {
        //faz se um bloqueio na Thread
        pthread_mutex_lock(&mutexProd);
        //verifica se a Thread termina
        if (posbfIN >= tamIN - 2) {
            pthread_mutex_unlock(&mutexProd);
            break;
        }
        //ciclo para adormecer a Thread produtora enquanto a Thread consumidora não consome algo
        while (ready >= tamCirc - 1) {
            pthread_cond_wait(&empty, &mutexProd); //colocar em espera threads produtoras enquanto o buffer_circ tiver cheio
        }
        //colocar no buffer_circ a media de 3 posiçoes consecutivas do buffer IN
        bfCirc[posbfIN % tamCirc] = (bfIN[posbfIN] + bfIN[posbfIN + 1] + bfIN[posbfIN + 2]) / 3; 
        //incrementar a psoiçao do buffer_in
        posbfIN++; 
        //incrementa o contador
        *((int*)count) += 1;
        //faz se o desbloqueio da Thread
        pthread_mutex_unlock(&mutexProd);

        //faz se um bloqueio na Thread
        pthread_mutex_lock(&mutexCons);
        //incrementa a variavel ready
        ready++;
        //acorda a Thread Consumidora quando estiver algo para esta consumir no buffer circ
        pthread_cond_signal(&full);
        //faz se o desbloqueio da Thread
        pthread_mutex_unlock(&mutexCons);

    }

}

void* consumer(void* count) {

    while (1) {
        //faz se um bloqueio na Thread
        pthread_mutex_lock(&mutexCons);
        //verifica se a Thread termina
        if (posbfOut == tamOUT) {
            pthread_mutex_unlock(&mutexCons);
            break;
        }
        //ciclo para adormecer a Thread consumidora enquanto uma das Thread produtoras não produz algo
        while (ready == 0) {
            pthread_cond_wait(&full, &mutexCons);
        }
        //retirar o valor do buffer circular e colocar no buffer OUT
        bfOUT[posbfOut] = bfCirc[posbfOut % tamCirc];
        //colocar o valor -1 na posiçao de onde se retirou o valor do buffer circular
        bfCirc[posbfOut % tamCirc] = -1; 
        //incrementar indice do buffer_out
        posbfOut++;
        //incrementa o contador
        *((int*)count) += 1;
        //decrementar a variavel ready
        ready--;
        //faz se o desbloqueio da Thread
        pthread_mutex_unlock(&mutexCons);

        //faz se um bloqueio na Thread
        pthread_mutex_lock(&mutexProd);

        //acorda uma Thread Produtora quando o buffer circ tiver pelo menos uma posição já consumida
        pthread_cond_signal(&empty);
        //faz se o desbloqueio da Thread
        pthread_mutex_unlock(&mutexProd);

    }

}
