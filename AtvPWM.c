#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define SERVO_PIN 22
#define PWM_FREQ 50  // Frequência do PWM em Hz
#define CLOCK_DIV 64 // Divisor de clock para gerar a frequência desejada

// Função para calcular o valor do ciclo ativo com base no tempo em microssegundos
uint16_t calc_duty_cycle(uint16_t pulse_width_us) {
    uint32_t clock_freq = clock_get_hz(clk_sys) / CLOCK_DIV;  // Frequência do clock dividida
    uint32_t wrap_value = clock_freq / PWM_FREQ;              // Valor de contagem máxima
    return (pulse_width_us * wrap_value) / 20000;             // Conversão para duty cycle
}

void set_servo_angle(uint16_t pulse_width_us) {
    pwm_set_gpio_level(SERVO_PIN, calc_duty_cycle(pulse_width_us));
}

int main() {
    stdio_init_all();

    // Configuração do pino GPIO como saída PWM
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);

    // Configuração do PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, CLOCK_DIV);
    pwm_config_set_wrap(&config, clock_get_hz(clk_sys) / CLOCK_DIV / PWM_FREQ);
    pwm_init(slice_num, &config, true);

    // Passo 1: Posição inicial de 180 graus (2400 µs)
    set_servo_angle(2400);
    sleep_ms(5000);  // Aguarda 5 segundos na posição de 180 graus

    // Passo 2: Posição de 90 graus (1470 µs)
    set_servo_angle(1470);
    sleep_ms(5000);  // Aguarda 5 segundos na posição de 90 graus

    // Passo 3: Posição de 0 graus (500 µs)
    set_servo_angle(500);
    sleep_ms(5000);  // Aguarda 5 segundos na posição de 0 graus

    // Loop infinito para movimento suave entre 0 e 180 graus
    while (true) {
        // Movimento suave de 0° para 180°
        for (uint16_t pulse = 500; pulse <= 2400; pulse += 5) {
            set_servo_angle(pulse);
            sleep_ms(10);  // Aguarda 10ms entre cada incremento
        }

        // Movimento suave de 180° para 0°
        for (uint16_t pulse = 2400; pulse >= 500; pulse -= 5) {
            set_servo_angle(pulse);
            sleep_ms(10);  // Aguarda 10ms entre cada decremento
        }
    }
}