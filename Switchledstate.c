#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Definição dos GPIOs dos LEDs e do Botão
#define LED_BLUE 11
#define LED_RED  12
#define LED_GREEN 13
#define BUTTON   5

// Variáveis globais de controle
volatile bool sequence_running = false;  
volatile int sequence_step = 0;          

// Estrutura para manter o temporizador repetitivo
struct repeating_timer timer;

// Função de callback do temporizador
bool turn_off_callback(struct repeating_timer *t) {
    switch (sequence_step) {
        case 0:
            // Desliga o LED azul
            gpio_put(LED_BLUE, 0);
            sequence_step++;
            break;
        case 1:
            // Desliga o LED vermelho
            gpio_put(LED_RED, 0);
            sequence_step++;
            break;
        case 2:
            // Desliga o LED verde
            gpio_put(LED_GREEN, 0);
            // Reinicia as variáveis
            sequence_step = 0;
            sequence_running = false;
            return false;  // Cancela o temporizador (não repete mais)
    }
    return true; 
}

// Função de callback da interrupção do botão
void button_callback(uint gpio, uint32_t events) {
    // Só inicia se não estiver em meio a uma sequência
    if (!sequence_running) {
        sequence_running = true;
        
        // Liga todos os LEDs
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        
        // Garante que o passo inicie do zero
        sequence_step = 0;
        
        // Inicia o temporizador repetitivo a cada 3 segundos (3000ms)
        add_repeating_timer_ms(3000, turn_off_callback, NULL, &timer);
    }
}

int main() {
    stdio_init_all();

    // Inicializa LEDs como saída
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    // Inicializa botão como entrada, com pull-up interno
    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_up(BUTTON);

    // Configura interrupção na borda de descida (quando o botão for pressionado)
    gpio_set_irq_enabled_with_callback(BUTTON, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    // Loop principal
    while (true) {
        sleep_ms(10);
    }

    return 0;
}
