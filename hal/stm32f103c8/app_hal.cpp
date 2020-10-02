#include "app_hal.h"
#include "app.h"

#include "main.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"

extern "C" void SystemClock_Config(void);


// Interrupt handlers should be un general namespace to override weak defaults.


// ADC data is transferred to double size DMA buffer. Interrupts happen on half
// transfer and full transfer. So, we can process received data without risk
// of override. While half of buffer is processed, another half os used to
// collect next data.

static volatile uint16_t ADCBuffer[ADC_FETCH_PER_TICK * ADC_CHANNELS_COUNT * 2];

// Resorted adc data for convenient use
static uint16_t adc_voltage_buf[ADC_FETCH_PER_TICK];
static uint16_t adc_current_buf[ADC_FETCH_PER_TICK];
static uint16_t adc_knob_buf[ADC_FETCH_PER_TICK];
static uint16_t adc_v_refin_buf[ADC_FETCH_PER_TICK];

// Split raw ADC data by separate buffers
static void adc_raw_data_load(uint32_t adc_data_offset)
{
    for (int sample = 0; sample < ADC_FETCH_PER_TICK; sample++)
    {
#ifdef REVERSE_VOLTAGE
        // When signal binded to VCC,
        // so substract it from VCC (max 12-bit value 0x0FFF)
        adc_voltage_buf[sample] = 0x0FFF - ADCBuffer[adc_data_offset++];
#else
        adc_voltage_buf[sample] = ADCBuffer[adc_data_offset++];
#endif
        adc_current_buf[sample] = ADCBuffer[adc_data_offset++];
        adc_knob_buf[sample] = ADCBuffer[adc_data_offset++];
        adc_v_refin_buf[sample] = ADCBuffer[adc_data_offset++];
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    (void)(AdcHandle);
    adc_raw_data_load(0);
    io.consume(adc_voltage_buf, adc_current_buf, adc_knob_buf, adc_v_refin_buf);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    (void)(AdcHandle);
    adc_raw_data_load(ADC_FETCH_PER_TICK * ADC_CHANNELS_COUNT);
    io.consume(adc_voltage_buf, adc_current_buf, adc_knob_buf, adc_v_refin_buf);
}


namespace hal {


void setup(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();

    triac_ignition_off();
}


void triac_ignition_on() {
#ifdef REVERSE_TRIAC
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
#else
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
#endif
}
void triac_ignition_off() {
#ifdef REVERSE_TRIAC
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
#endif
}


void start() {
    // Final hardware start: calibrate ADC & run cyclic DMA ops.
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADCBuffer, ADC_FETCH_PER_TICK * ADC_CHANNELS_COUNT * 2);
}

}
