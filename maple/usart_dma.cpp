/**
 * @file examples/test-usart-dma.cpp
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 *
 * Simple test of DMA used with a USART transmitter.
 *
 * Configures a USART transmitter for use with DMA.
 * Buffer is filled with characters, then transmitted 
 * over and over in a loop using DMA.
 *
 */

#include <libmaple/dma.h>
#include <libmaple/usart.h>
#include <libmaple/gpio.h>
#include <wirish/wirish.h>

/*
 * Configuration and state
 */

// Serial port and DMA configuration. You can change these to suit
// your purposes.
HardwareSerial *serial = &Serial1;
#define USART_DMA_DEV DMA1
#if (STM32_MCU_SERIES == STM32_SERIES_F1 || \
     STM32_MCU_SERIES == STM32_SERIES_F3)                  
// On STM32F1 microcontrollers (like what's on Maple and Maple Mini),
// dma tubes are channels.
#define USART_TX_DMA_TUBE DMA_CH4
#else
#error "unsupported stm32 series"
#endif
// The serial port will make a DMA request each time it wants to transmit data.
// This is the dma_request_src we use to tell the DMA tube to handle
// that DMA request.
#define USART_DMA_REQ_SRC DMA_REQ_SRC_USART1_TX
#define BAUD 8500000

// This will store the DMA configuration for USART TX.
dma_tube_config tube_config;

// This will store characters to transmit to the USART.
#define BUF_SIZE 1000 
char tx_buf[BUF_SIZE];

// The interrupt handler, tx_dma_irq(), sets this to 1.
volatile uint32 irq_fired = 0;
// Used to store DMA interrupt status register (ISR) bits inside
// tx_dma_irq(). This helps explain what's going on inside loop(); see
// comments below.
volatile uint32 isr = 0;

/*
 * Helper functions
 */

// This is our DMA interrupt handler.
void tx_dma_irq(void) {
    irq_fired = 1;
    isr = dma_get_isr_bits(USART_DMA_DEV, USART_TX_DMA_TUBE);
}

// Configure the USART receiver for use with DMA:
// 1. Turn it on.
// 2. Set the "DMA request on TX" bit in USART_CR3 (USART_CR3_DMAT).
void setup_usart(void) {
    serial->begin(BAUD);
    usart_dev *serial_dev = serial->c_dev();
    serial_dev->regs->CR3 = USART_CR3_DMAT;
}

// Set up our dma_tube_config structure. (We could have done this
// above, when we declared tube_config, but having this function makes
// it easier to explain what's going on).
void setup_tube_config(void) {
    // We're transmitting from tx_buf.
    tube_config.tube_src = tx_buf;
    // We're only interested in the bottom 8 bits of that data register.
    tube_config.tube_src_size = DMA_SIZE_8BITS;
    // We're storing to the USART data register. serial->c_dev()
    // returns a pointer to the libmaple usart_dev for that serial
    // port, so this is a pointer to its data register.
    tube_config.tube_dst = &serial->c_dev()->regs->TDR;
    // tx_buf is a char array, and a "char" takes up 8 bits on STM32.
    tube_config.tube_dst_size = DMA_SIZE_8BITS;
    // transmit BUF_SIZE characters
    tube_config.tube_nr_xfers = BUF_SIZE;
    // Flags:
    // - DMA_CFG_DST_INC so we start at the beginning of tx_buf and
    //   transmit towards the end.
    // - DMA_CFG_CMPLT_IE to turn on interrupts on transfer completion.
    tube_config.tube_flags = DMA_CFG_SRC_INC | DMA_CFG_CIRC | DMA_CFG_CMPLT_IE;
    // Target data: none. It's important to set this to NULL if you
    // don't have any special (microcontroller-specific) configuration
    // in mind, which we don't.
    tube_config.target_data = NULL;
    // DMA request source.
    tube_config.tube_req_src = USART_DMA_REQ_SRC;
}

// Configure the DMA controller to serve DMA requests from the USART.
void setup_dma_xfer(void) {
    // First, turn it on.
    dma_init(USART_DMA_DEV);
    // Next, configure it by calling dma_tube_cfg(), and check to make
    // sure it succeeded. DMA tubes have many restrictions on their
    // configuration, and there are configurations which work on some
    // types of STM32 but not others. libmaple tries hard to make
    // things just work, but checking the return status is important!
    int status = dma_tube_cfg(USART_DMA_DEV, USART_TX_DMA_TUBE, &tube_config);
    ASSERT(status == DMA_TUBE_CFG_SUCCESS);
    // Now we'll perform any other configuration we want. For this
    // example, we attach an interrupt handler.
    dma_attach_interrupt(USART_DMA_DEV, USART_TX_DMA_TUBE, tx_dma_irq);
    // Turn on the DMA tube. It will now begin serving requests.
    dma_enable(USART_DMA_DEV, USART_TX_DMA_TUBE);
}

/*
 * setup() and loop()
 */

void setup(void) {
    pinMode(BOARD_LED_PIN, OUTPUT);

    for (int i=0; i<BUF_SIZE; i++) {
       tx_buf[i] = 'x';
    }

    setup_tube_config();
    setup_dma_xfer();
    setup_usart();
}

void loop(void) {
    delay(100);
    //toggleLED();

    // See if the interrupt handler got called since the last time we
    // checked.
    if (irq_fired) {
        irq_fired = 0;
        toggleLED();
    }

}

// ------- init() and main() --------------------------------------------------

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain() {
    init();
}

int main(void) {
    setup();

    while (true) {
        loop();
    }
    return 0;
}

