#include "aquisicao.h"
#include "usart.h"
#include "tim.h"
#include "dma.h"
#include "adc.h"
#include "cmsis_os.h"

osThreadId handle_tarefa_aquisicao;
QueueHandle_t amostras;
uint16_t amostras_dma[AQUISICAO_AMOSTRAS];

void aquisicao_inicializar_tarefas()
{
    osThreadDef(tarefa_aquisicao_nome, aquisicao_tarefa, osPriorityNormal, 0, AQUISICAO_STACK_SIZE);
    handle_tarefa_aquisicao = osThreadCreate(osThread(tarefa_aquisicao_nome), NULL);
}

void aquisicao_inicializar_listas()
{
    amostras = xQueueCreate(AQUISICAO_AMOSTRAS, sizeof(uint16_t));
}

void aquisicao_inicializar_perifericos()
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)amostras_dma, AQUISICAO_AMOSTRAS);
    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
}

static void aquisicao_tarefa()
{
    char buffer[64];
    uint16_t pvt_amostras[AQUISICAO_AMOSTRAS];
    size_t len_msg;
    for (;;)
    {
        if (xQueueReceive(amostras, pvt_amostras, 10000) == pdPASS)
        {
            len_msg = sprintf(buffer, "%i\n", pvt_amostras[0]);
            HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len_msg, 1000);
        }        
    }
}