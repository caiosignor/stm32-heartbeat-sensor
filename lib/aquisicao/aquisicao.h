#ifndef AQUISICAO_H
#define AQUISICAO_H
#include "cmsis_os.h"
#include "usart.h"

#define AQUISICAO_AMOSTRAS 64
#define AQUISICAO_FREQ_MAX 3
#define AQUISICAO_FREQ_SMP FREQ_MAX * AMOSTRAS

#define AQUISICAO_STACK_SIZE 256

extern osThreadId handle_tarefa_aquisicao;
extern QueueHandle_t amostras;
extern uint16_t amostras_dma[AQUISICAO_AMOSTRAS];

void aquisicao_inicializar_tarefas();
void aquisicao_inicializar_listas();
void aquisicao_inicializar_perifericos();
static void aquisicao_tarefa();

#endif