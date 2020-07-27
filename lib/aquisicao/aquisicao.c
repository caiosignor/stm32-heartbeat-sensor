#include "aquisicao.h"
#include "usart.h"
#include "tim.h"
#include "dma.h"
#include "adc.h"
#include "cmsis_os.h"
#define ARM_MATH_CM3
#include "arm_math.h"
#include "arm_const_structs.h"

osThreadId handle_tarefa_aquisicao;
uint16_t amostras_dma[AQUISICAO_AMOSTRAS];
float32_t amostras_pvt[AQUISICAO_AMOSTRAS / 2];

osSemaphoreId consumidor;   // Semaphore ID
osSemaphoreDef(consumidor); // Semaphore definition
osSemaphoreId produtor;   // Semaphore ID
osSemaphoreDef(produtor); // Semaphore definition

void aquisicao_inicializar_tarefas()
{
    osThreadDef(tarefa_aquisicao_nome, aquisicao_tarefa, osPriorityNormal, 0, AQUISICAO_STACK_SIZE/4);
    handle_tarefa_aquisicao = osThreadCreate(osThread(tarefa_aquisicao_nome), NULL);
}

void aquisicao_inicializar_listas()
{
    consumidor = osSemaphoreCreate(osSemaphore(consumidor), 1);
    produtor = osSemaphoreCreate(osSemaphore(produtor), 1);
    osSemaphoreRelease(produtor);
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
    float32_t sinal[AQUISICAO_AMOSTRAS / 2];
    for (;;)
    {
        osSemaphoreWait(consumidor, 10000);
        arm_copy_f32(amostras_pvt, sinal, AQUISICAO_AMOSTRAS / 2);
        arm_cfft_f32(&arm_cfft_sR_f32_len64, sinal, 0, 1);
        arm_cmplx_mag_f32(sinal, sinal, 64);

        for (int i = 0; i < 64; i++)
        {
            len_msg = sprintf(buffer, "%.0f\n", sinal[i]);          
            HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len_msg, 1000);
        }
        osSemaphoreRelease(produtor);
    }
}