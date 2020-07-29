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
uint16_t amostras_dma[DMA_BUFFER];
float32_t amostras_pvt[TAMANHO_VETOR];

osSemaphoreId consumidor;   // Semaphore ID
osSemaphoreDef(consumidor); // Semaphore definition
osSemaphoreId produtor;     // Semaphore ID
osSemaphoreDef(produtor);   // Semaphore definition

void aquisicao_inicializar_tarefas()
{
    osThreadDef(tarefa_aquisicao_nome, aquisicao_tarefa, osPriorityNormal, 0, AQUISICAO_STACK_SIZE / 4);
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
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)amostras_dma, DMA_BUFFER);
    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
}

static void aquisicao_tarefa()
{
    char buffer[64];
    size_t len_msg;
    float32_t fft_abs[AMOSTRAS_FFT];
    for (;;)
    {
        osSemaphoreWait(consumidor, 10000);
        // arm_scale_f32(amostras_pvt, (3.3 / 4096), amostras_pvt, TAMANHO_VETOR);
        // arm_cfft_f32(&arm_cfft_sR_f32_len256, amostras_pvt, 0, 1);
        // arm_cmplx_mag_f32(amostras_pvt, fft_abs, AMOSTRAS_FFT);

        // fft_abs[0] = 0;
        len_msg = sprintf(buffer, "@\n");
        HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len_msg, 1000);
        for (int i = 0; i < AMOSTRAS_FFT; i++)
        {
            len_msg = sprintf(buffer, "%.0f\n", amostras_pvt[i]);
            HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len_msg, 1000);
        }
        len_msg = sprintf(buffer, "#\n");
        HAL_UART_Transmit(&huart1, (uint8_t *)buffer, len_msg, 1000);

        osSemaphoreRelease(produtor);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    osSemaphoreWait(produtor, 10000);
    static q15_t contador = 0;
    for (int i = 0; i < DMA_BUFFER; i++)
        amostras_pvt[(contador == 0 ? 0 : 256) + i] = (float32_t)amostras_dma[i];

    if (contador++ == 2)
    {
        contador = 0;
        osSemaphoreRelease(consumidor);
    }
}