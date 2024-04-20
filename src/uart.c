// -----------------------------------uart.c -------------------------------------
#include "uart.h"
#include "gpio.h"

// #include "promptProcess.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init(unsigned int baud_rate, int data_bit)
{
    unsigned int r;

    /* Turn off UART0 */
    UART0_CR = 0x0;

    /* Setup GPIO pins 14 and 15 */

    /* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
    r = GPFSEL1;
    r &= ~((7 << 12) | (7 << 15));      // clear bits 17-12 (FSEL15, FSEL14)
    r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
    GPFSEL1 = r;

    /* enable GPIO 14, 15 */
#ifdef RPI3    // RPI3
    GPPUD = 0; // No pull up/down control
    // Toogle clock to flush GPIO setup
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
    }              // waiting 150 cycles
    GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
    r = GPIO_PUP_PDN_CNTRL_REG0;
    r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
    GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

    /* Mask all interrupts. */
    UART0_IMSC = 0;

    /* Clear pending interrupts. */
    UART0_ICR = 0x7FF;

    /* Set integer & fractional part of Baud rate
    Divider = UART_CLOCK/(16 * Baud)
    Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
    Integer part register UART0_IBRD  = integer part of Divider
    Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

    float baudrate_divisor = (float)48000000 / (16 * baud_rate);
    int integer_part = (int)baudrate_divisor;
    int fraction_part = (int)((float)(baudrate_divisor - integer_part) * 64 + 0.5);
    // 115200 baud
    UART0_IBRD = integer_part;
    UART0_FBRD = fraction_part;

    /* Set up the Line Control Register */
    /* Enable FIFO */
    /* Set length to 8 bit */
    /* Defaults for other bit are No parity, 1 stop bit */
    if (data_bit == 5)
    {
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_5BIT;
    }
    else if (data_bit == 6)
    {
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_6BIT;
    }
    else if (data_bit == 7)
    {
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_7BIT;
    }
    else if (data_bit == 8)
    {
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;
    }

    /* Enable UART0, receive, and transmit */
    UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

/* Configure word length */
void data_bits_configuration(int data_bits)
{
    // Clear the word length bits in LCRH
    UART0_LCRH &= ~UART0_LCRH_WLEN_CLR;

    // Set the appropriate word length based on the input
    switch (data_bits)
    {
    case 5:
        UART0_LCRH |= UART0_LCRH_WLEN_5BIT;
        break;
    case 6:
        UART0_LCRH |= UART0_LCRH_WLEN_6BIT;
        break;
    case 7:
        UART0_LCRH |= UART0_LCRH_WLEN_7BIT;
        break;
    case 8:
        UART0_LCRH |= UART0_LCRH_WLEN_8BIT;
        break;
    }
}

int uart_set_baudrate(char *s)
{
    int baudrate = 0;

    // Convert the string to an integer manually
    while (*s != '\0')
    {
        if (!(*s >= '0' && *s <= '9'))
        {
            // If any character is not a digit, return false
            uart_puts("Invalid choice\n");
            uart_puts("Enter another choice: ");
            return 0;
        }
        baudrate = baudrate * 10 + (*s - '0');
        s++;
    }

    switch (baudrate)
    {
    case 1:
        uart_init(9600, 6);
        break;
    case 2:
        uart_init(19200, 6);
        break;
    case 3:
        uart_init(38400, 6);
        break;
    case 4:
        uart_init(57600, 6);
        break;
    case 5:
        uart_init(115200, 6);
        break;

    default:
        uart_puts("Invalid choice\n");
        uart_puts("Enter another choice: ");
        return 0;
    }

    return 1;
}

/**
 * Send a character
 */
void uart_sendc(char c)
{
    /* Check Flags Register */
    /* And wait until transmitter is not full */
    do
    {
        asm volatile("nop");
    } while (UART0_FR & UART0_FR_TXFF);

    /* Write our data byte out to the data register */
    UART0_DR = c;
}

/**
 * Receive a character
 */
char uart_getc()
{
    char c = 0;

    /* Check Flags Register */
    /* Wait until Receiver is not empty
     * (at least one byte data in receive fifo)*/
    do
    {
        asm volatile("nop");
    } while (UART0_FR & UART0_FR_RXFE);

    /* read it and return */
    c = (unsigned char)(UART0_DR);

    /* convert carriage return to newline */
    return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
// void uart_puts(char *s, char *t_color, char *b_color)
void uart_puts(char *s)
{
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
 * Display a value in hexadecimal format
 */
void uart_mac_address_hex(unsigned int address1, unsigned int address2)
{
    int cnt = 0;
    for (int pos = 14; pos >= 0; pos = pos - 4)
    {
        // Get highest 4-bit nibble
        char digit = (address2 >> pos) & 0xF;
        /* Convert to ASCII code */
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        digit += (digit > 9) ? (-10 + 'A') : '0';
        uart_sendc(digit);
        cnt++;
        if (cnt % 2 == 0)
        {
            uart_sendc(':');
        }
    }
    for (int pos = 28; pos >= 0; pos = pos - 4)
    {
        // Get highest 4-bit nibble
        char digit = (address1 >> pos) & 0xF;
        /* Convert to ASCII code */
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        digit += (digit > 9) ? (-10 + 'A') : '0';
        uart_sendc(digit);
        cnt++;
        if (cnt % 2 == 0 && cnt <= 10)
        {
            uart_sendc(':');
        }
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
    uart_sendc('\r');
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

void uart_set_t_color()
{
    switch (uart_t_color)
    {
    case 0:
        uart_puts(ANSI_COLOR_FG_BLACK);
        break;

    case 1:
        uart_puts(ANSI_COLOR_FG_RED);
        break;

    case 2:
        uart_puts(ANSI_COLOR_FG_GREEN);
        break;

    case 3:
        uart_puts(ANSI_COLOR_FG_YELLOW);
        break;

    case 4:
        uart_puts(ANSI_COLOR_FG_BLUE);
        break;

    case 5:
        uart_puts(ANSI_COLOR_FG_MAGENTA);
        break;

    case 6:
        uart_puts(ANSI_COLOR_FG_CYAN);
        break;

    default:
        uart_puts(ANSI_COLOR_FG_WHITE);
        break;
    }
}

void uart_set_b_color()
{
    switch (uart_b_color)
    {
    case 0:
        uart_puts(ANSI_COLOR_BG_BLACK);
        break;

    case 1:
        uart_puts(ANSI_COLOR_BG_RED);
        break;

    case 2:
        uart_puts(ANSI_COLOR_BG_GREEN);
        break;

    case 3:
        uart_puts(ANSI_COLOR_BG_YELLOW);
        break;

    case 4:
        uart_puts(ANSI_COLOR_BG_BLUE);
        break;

    case 5:
        uart_puts(ANSI_COLOR_BG_MAGENTA);
        break;

    case 6:
        uart_puts(ANSI_COLOR_BG_CYAN);
        break;

    default:
        uart_puts(ANSI_COLOR_BG_WHITE);
        break;
    }
    uart_puts("\e[K");
}