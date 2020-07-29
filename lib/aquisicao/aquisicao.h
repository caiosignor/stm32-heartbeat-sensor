#ifndef AQUISICAO_H
#define AQUISICAO_H
#define ARM_MATH_CM3
#include "arm_math.h"

#include "cmsis_os.h"
#include "usart.h"

#define FREQUENCIA_BASE 0.5 //Hz - 30bpm
#define NUMERO_AMOSTRAS 256
#define FREQUENCIA_AMS FREQUENCIA_BASE *NUMERO_AMOSTRAS
#define DMA_BUFFER NUMERO_AMOSTRAS
#define NUMERO_CICLOS 2
#define TAMANHO_VETOR NUMERO_CICLOS *NUMERO_AMOSTRAS
#define AMOSTRAS_FFT TAMANHO_VETOR / 2
#define AQUISICAO_STACK_SIZE 5000

extern osThreadId handle_tarefa_aquisicao;
extern osSemaphoreId consumidor;
extern osSemaphoreId produtor;
extern uint16_t amostras_dma[DMA_BUFFER];
float32_t amostras_pvt[TAMANHO_VETOR];

void aquisicao_inicializar_tarefas();
void aquisicao_inicializar_listas();
void aquisicao_inicializar_perifericos();
static void aquisicao_tarefa();

#endif