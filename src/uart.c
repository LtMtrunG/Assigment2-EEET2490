// -----------------------------------uart.c -------------------------------------
#include "uart.h"
#include "gpio.h"
//#include "promptProcess.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    unsigned int r;
    /* initialize UART */
    AUX_ENABLE |= 1;   // enable mini UART (UART1)
    AUX_MU_CNTL = 0;   // stop transmitter and receiver
    AUX_MU_LCR = 3;    // 8-bit mode (also enable bit 1 to be used for RBP3)
    AUX_MU_MCR = 0;    // clear RTS (request to send)
    AUX_MU_IER = 0;    // disable interrupts
    AUX_MU_IIR = 0xc6; // enable and clear FIFOs
    AUX_MU_BAUD = 270; // configure 115200 baud [system_clk_freq/(baud_rate*8) - 1]
    /* map UART1 to GPIO pins 14 and 15 */
    r = GPFSEL1;
    r &= ~((7 << 12) | (7 << 15)); // clear bits 17-12 (FSEL15, FSEL14)
    r |= (2 << 12) | (2 << 15);    // set value 2 (select ALT5: TXD1/RXD1)
    GPFSEL1 = r;
    /* enable GPIO 14, 15 */
    GPPUD = 0; // No pull up/down control
    r = 150;
    while (r--)
    {
        asm volatile("nop");
    }                                  // waiting 150 cycles
    GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
    r = 150;
    while (r--)
    {
        asm volatile("nop");
    }                // waiting 150 cycles
    GPPUDCLK0 = 0;   // flush GPIO setup
    AUX_MU_CNTL = 3; // enable transmitter and receiver (Tx, Rx)
}
/**
 * Send a character
 */
void uart_sendc(char c)
{
    // wait until transmitter is empty
    do
    {
        asm volatile("nop");
    } while (!(AUX_MU_LSR & 0x20));
    // write the character to the buffer
    AUX_MU_IO = c;
}
/**
 * Receive a character
 */
char uart_getc()
{
    char c;
    // wait until data is ready (one symbol)
    do
    {
        asm volatile("nop");
    } while (!(AUX_MU_LSR & 0x01));
    // read it and return
    c = (unsigned char)(AUX_MU_IO);
    // convert carriage return to newline character
    return (c == '\r' ? '\n' : c);
}
/**
 * Display a string
 */
//void uart_puts(char *s, char *t_color, char *b_color)
void uart_puts(char *s)
{
    char *color = ANSI_COLOR_BG_BLUE;
    while (*color) 
    {
        if (*color == '\n')
            uart_sendc('\r');
        uart_sendc(*color++);
    }
    while (*s)
    {
        // convert newline to carriage return + newline
        if (*s == '\n')
            uart_sendc('\r');
        uart_sendc(*s++);
    }
}

/**
 * Display a value in hexadecimal format
 */
void uart_hex(unsigned int num)
{
    uart_puts("0x");
    for (int pos = 28; pos >= 0; pos = pos - 4)
    {
        // Get highest 4-bit nibble
        char digit = (num >> pos) & 0xF;
        /* Convert to ASCII code */
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        digit += (digit > 9) ? (-10 + 'A') : '0';
        uart_sendc(digit);
    }
}
/**
 * Display a value in decimal format
 */
void uart_dec(int num)
{
    // A string to store the digit characters
    char str[33] = "";
    // Calculate the number of digits
    int len = 1;
    int temp = num;
    while (temp >= 10)
    {
        len++;
        temp = temp / 10;
    }
    // Store into the string and print out
    for (int i = 0; i < len; i++)
    {
        int digit = num % 10; // get last digit
        num = num / 10;       // remove last digit from the number
        str[len - (i + 1)] = digit + '0';
    }
    str[len] = '\0';
    uart_puts(str);
}

void uart_display_welcome_screen()
{
    uart_puts("oooooooooooo oooooooooooo oooooooooooo ooooooooooooo   .oooo.         .o    .ooooo.     .oooo.   \n");
    uart_puts("`888'     `8 `888'     `8 `888'     `8 8'   888   `8 .dP  Y88b      .d88   888' `Y88.  d8P'`Y8b  \n");
    uart_puts(" 888          888          888              888            ]8P'   .d'888   888    888 888    888 \n");
    uart_puts(" 888oooo8     888oooo8     888oooo8         888          .d8P'  .d'  888    `Vbood888 888    888 \n");
    uart_puts(" 888          888          888              888        .dP'     88ooo888oo       888' 888    888 \n");
    uart_puts(" 888       o  888       o  888       o      888      .oP     .o      888       .88P'  `88b  d88' \n");
    uart_puts("o888ooooood8 o888ooooood8 o888ooooood8     o888o     8888888888     o888o    .oP'      `Y8bd8P'  \n");
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("oooooooooo.        .o.       ooooooooo.   oooooooooooo      .oooooo.    .oooooo..o               \n");
    uart_puts("`888'   `Y8b      .888.      `888   `Y88. `888'     `8     d8P'  `Y8b  d8P'    `Y8               \n");
    uart_puts(" 888     888     .8  88.      888   .d88'  888            888      888 Y88bo.                    \n");
    uart_puts(" 888oooo888'    .8' `888.     888ooo88P'   888oooo8       888      888   `Y8888o.                \n");
    uart_puts(" 888    `88b   .88ooo8888.    888`88b.     888            888      888       `Y88b               \n");
    uart_puts(" 888    .88P  .8'     `888.   888  `88b.   888       o    `88b    d88' oo     .d8P               \n");
    uart_puts("o888bood8P'  o88o     o8888o o888o  o888o o888ooooood8     `Y8bood8P'  888888P'                  \n");
    uart_puts("\n");
    uart_puts("    Developed by Le Tran Minh Trung - s3927071\n");
    uart_puts("\n");
    uart_puts("Type help to show brief information of all commands\n");
    uart_puts("\n");
}

void uart_clear_screen()
{
    uart_puts("\033[2J\033[1;1H");
}

void uart_display_os()
{
    uart_puts("CloudOS> ");
}

void uart_puts_clear_line(char *s)
{
    // Clear current line
    uart_sendc('\r');
    uart_sendc('\033'); // Escape character for ANSI escape sequences
    uart_sendc('[');
    uart_sendc('K');

    uart_display_os();

    // Display the string
    while (*s)
    {
        // convert newline to carriage return + newline
        if (*s == '\n')
        {
            uart_sendc('\r');
            uart_sendc('\n');
        }
        else
        {
            uart_sendc(*s);
        }
        s++;
    }
}
/*
void uart_process_prompt(char *prompt, int size)
{
    if (is_help(prompt))
    {
        if (size == 4)
        {
            uart_puts("List of available commands:\n");
            uart_puts("clear\n");
            uart_puts("setcolor\n   -t <text color>\n   -t <background color>\n");
            uart_puts("showinfo\n");
            uart_puts("\n");
            uart_puts("For more information on a specific command, type help command-name");
        }
        else if (is_help_clear(prompt))
        {
            uart_puts("clear            clear the screen and bring the command line on top of the terminal\n");
        }
        else if (is_help_setcolor(prompt))
        {
            uart_puts("setcolor         set text color, and/or background color of the console to one of the following colors: black, red, green, yellow, blue, purple, cyan, white\n");
        }
        else if (is_help_showinfo(prompt))
        {
            uart_puts("showinfo         show board revision and board MAC address in correct format/ meaningful information\n");
        }
        else
        {
            uart_puts("Invalid command! Type help to see available commands\n");
        }
    } else
    {
        uart_puts("Invalid command!");
    }
}*/