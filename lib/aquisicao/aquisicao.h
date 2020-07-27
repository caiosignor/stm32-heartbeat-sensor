#ifndef AQUISICAO_H
#define AQUISICAO_H
#define ARM_MATH_CM3
#include "arm_math.h"

#include "cmsis_os.h"
#include "usart.h"

#define AQUISICAO_AMOSTRAS 256
#define AQUISICAO_FREQ_MAX 3
#define AQUISICAO_FREQ_SMP FREQ_MAX *AMOSTRAS
#define AQUISICAO_STACK_SIZE 2048
#define FFT_SIZE AQUISICAO_AMOSTRAS / 2

extern osThreadId handle_tarefa_aquisicao;
extern osSemaphoreId consumidor;
extern osSemaphoreId produtor;   
extern uint16_t amostras_dma[AQUISICAO_AMOSTRAS];
float32_t amostras_pvt[AQUISICAO_AMOSTRAS / 2];

void aquisicao_inicializar_tarefas();
void aquisicao_inicializar_listas();
void aquisicao_inicializar_perifericos();
static void aquisicao_tarefa();

#endif