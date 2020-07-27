#include "aquisicao.h"
#include "usart.h"
#include "tim.h"
#include "dma.h"
#include "adc.h"
#include "cmsis_os.h"
#define ARM_MATH_CM3
#include "arm_math.h"

osThreadId handle_tarefa_aquisicao;
uint16_t amostras_dma[AQUISICAO_AMOSTRAS];
float32_t amostras_pvt[AQUISICAO_AMOSTRAS / 2];

osSemaphoreId consumidor;   // Semaphore ID
osSemaphoreDef(consumidor); // Semaphore definition

void aquisicao_inicializar_tarefas()
{
    osThreadDef(tarefa_aquisicao_nome, aquisicao_tarefa, osPriorityNormal, 0, AQUISICAO_STACK_SIZE);
    handle_tarefa_aquisicao = osThreadCreate(osThread(tarefa_aquisicao_nome), NULL);
}

void aquisicao_inicializar_listas()
{
    consumidor = osSemaphoreCreate(osSemaphore(consumidor), 1);
}

void aquisicao_inicializar_perifericos()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)amostras_dma, AQUISICAO_AMOSTRAS);
    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
}

static void aquisicao_tarefa()
{
    char buffer[64];
    size_t len_msg;
    for (;;)
    {
        osSemaphoreWait(consumidor, 10000);
        for (int i = 0; i < AQUISICAO_AMOSTRAS / 2; i++)
        {
            len_msg = sprintf(buffer, "%.0f\n", amostras_pvt[i]);
            HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len_msg, 1000);
        }
    }
}