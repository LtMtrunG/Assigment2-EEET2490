// -----------------------------------uart.c -------------------------------------
#include "uart.h"
#include "gpio.h"

// #include "promptProcess.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
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

    float baudrate_divisor = (float)48000000 / (16 * baudrate);
    int integer_part = (int)baudrate_divisor;
    int fraction_part = (int)((float)(baudrate_divisor - integer_part) * 64 + 0.5);
    UART0_IBRD = integer_part;
    UART0_FBRD = fraction_part;

    // Set data bits
    switch (data_bits)
    {
    case 5: // Set 5 data bits
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_5BIT; // Enable FIFO and set 5 bits length
        break;
    case 6: // Set 6 data bits
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_6BIT; // Enable FIFO and set 6 bits length
        break;
    case 7: // Set 7 data bits
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_7BIT; // Enable FIFO and set 7 bits length
        break;
    default: // Set 8 data bits
        UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT; // Enable FIFO and set 8 bits length
        break;
    }
 
    // Set stop bits
    switch (stop_bits)
    {
    case 1: // Set 1 stop bit
        UART0_LCRH &= ~UART0_LCRH_CTP; // Clear STP2 bit (default 1 stop bit)
        break;
    default: // Set 2 stop bits
        UART0_LCRH |= UART0_LCRH_STP2; // Set STP2 bit for 2 stop bits
        break;
    }

    // Set parity
    switch (parity)
    {
    case 'n': // Set none parity
        UART0_LCRH &= ~(1 << 1); // Disable PEN (parity enable)
        break;
    case 'e': // Set even parity
        UART0_LCRH |= UART0_LCRH_PEN; // Enable PEN (parity enable)
        UART0_LCRH |= UART0_LCRH_EPS; // Set EPS bit for even parity
        break;
    default: // Set odd parity
        UART0_LCRH |= UART0_LCRH_PEN;  // Enable PEN (parity enable)
        UART0_LCRH &= ~UART0_LCRH_EPS; // Clear EPS bit for odd parity
        break;
    }

    /* Enable UART0, receive, and transmit */
    UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

int uart_set_baudrate(char *s)
{
    //int baudrate_prompt = is_number(s); // Convert prompt to number

    switch (*s)
    {
    case '1': // Set baud rate variable to 9600
        baudrate = 9600;
        break;
    case '2': // Set baud rate variable to 19200
        baudrate = 19200;
        break;
    case '3': // Set baud rate variable to 38400
        baudrate = 38400;
        break;
    case '4': // Set baud rate variable to 57600
        baudrate = 57600;
        break;
    case '5': // Set baud rate variable to 115200
        baudrate = 115200;
        break;

    default: // Invalid choice
        uart_puts("Invalid choice\n");
        uart_puts("Enter another choice: ");
        return 0;
    }

    // Restart uart with new configuration
    uart_init();
    return 1;
}

int uart_set_data_bits(char *s)
{
    int data_bits_prompt = is_number(s); // Convert prompt to number

    switch (data_bits_prompt)
    {
    case 5: // Set data bits variable to 5
        data_bits = 5;
        break;
    case 6: // Set data bits variable to 6
        data_bits = 6;
        break;
    case 7: // Set data bits variable to 7
        data_bits = 7;
        break;
    case 8: // Set data bits variable to 8
        data_bits = 8;
        break;

    default: // Invalid choice
        uart_puts("Invalid choice\n");
        uart_puts("Enter another choice: ");
        return 0;
    }

    // Restart uart with new configuration
    uart_init();
    return 1;
}

int uart_set_stop_bits(char *s)
{
    int stop_bits_prompt = is_number(s); // Convert prompt to number

    switch (stop_bits_prompt)
    {
    case 1: // Set stop bits variable to 1
        stop_bits = 1;
        break;
    case 2: // Set stop bits variable to 2
        stop_bits = 2;
        break;

    default: // Invalid choice
        uart_puts("Invalid choice\n");
        uart_puts("Enter another choice: ");
        return 0;
    }

    // Restart uart with new configuration
    uart_init();
    return 1;
}

int uart_set_parity(char *s)
{
    char parity_prompt = '\0'; 
    if (*(s + 1) == '\0') { // Check prompt only one character
        parity_prompt = *s;
    }
    switch (parity_prompt)
    {
    case 'n': // Set parity variable to none
        parity = 'n';
        break;
    case 'e': // Set parity variable to even
        parity = 'e';
        break;
    case 'o': // Set parity variable to odd
        parity = 'o';
        break;
    default: // Invalid choice
        uart_puts("Invalid choice\n");
        uart_puts("Enter another choice: ");
        return 0;
    }

    // Restart uart with new configuration
    uart_init();
    return 1;
}

int uart_set_handshaking(char *s)
{
    // Disable transmit and receive
    UART0_CR &= ~UART0_CR_TXE;
    UART0_CR &= ~UART0_CR_RXE;

    // Flush the transmit FIFO
    UART0_LCRH &= ~UART0_LCRH_FEN;

    char handshaking_prompt = '\0'; 
    if (*(s + 1) == '\0') { // Check prompt only one character
        handshaking_prompt = *s;
    }

    // Reprogram the Control Register, UART_CR
    switch (handshaking_prompt)
    {
    case 'c': // CTS hardware flow control enable
        UART0_CR |= UART0_CR_CTSEN;
        break;
    case 'r': // RTS hardware flow control enable
        UART0_CR |= UART0_CR_RTSEN;
        break;
    default: // Invalid choice
        uart_puts("Invalid choice\n");
        uart_puts("Enter another choice: ");
        return 0;
    }

    // Enable transmit and receive
    UART0_CR |= UART0_CR_TXE;
    UART0_CR |= UART0_CR_RXE;

    return 1;
}

int is_number(char *s)
{
    int temp = 0;

    // Convert the string to an integer manually
    while (*s != '\0')
    {
        if (!(*s >= '0' && *s <= '9'))
        {
            // If any character is not a digit, return false
            return -1;
        }
        temp = temp * 10 + (*s - '0');
        s++;
    }

    return temp;
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

// Function to calculate parity bit
int get_parity(char data)
{
    int count = 0;

    // Count the number of set bits in the data
    while (data)
    {
        count += data & 1;
        data >>= 1;
    }

    // Return 1 for odd parity, 0 for even parity
    return count % 2;
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

    // if (parity == 'e')
    // {
    //     // Even parity
    //     if (get_parity(c) != 0)
    //     {
    //         uart_puts("Even parity error detected!\n");
    //     }
    // }
    // else if (parity == 'o')
    // {
    //     // Odd parity
    //     if (get_parity(c) == 0)
    //     {
    //         uart_puts("Odd parity error detected!\n");
    //     }
    // }

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
 * Display board revision
 */
void uart_print_board_revision(uint32_t revision)
{
    switch (revision)
    {
    case 0x00A02082:
        uart_puts("Pi 3 Model B Rev 1.2 1GB RAM (Sony, UK)");
        break;
    case 0x00A22082:
        uart_puts("Pi 3 Model B Rev 1.2 1GB RAM (Embest, China)");
        break;
    case 0x00A32082:
        uart_puts("Pi 3 Model B Rev 1.2 1GB RAM (Sony, Japan)");
        break;
    case 0x00A020D3:
        uart_puts("Pi 3 Model B+ Rev 1.3 1GB RAM (Sony, UK)");
        break;
    case 0x00A03111:
        uart_puts("Pi 4 Model B Rev 1.1 1GB RAM (Sony, UK)");
        break;
    case 0x00B03111:
        uart_puts("Pi 4 Model B Rev 1.1 2GB RAM (Sony, UK)");
        break;
    case 0x00B03112:
        uart_puts("Pi 4 Model B Rev 1.2 2GB RAM (Sony, UK)");
        break;
    case 0x00B03114:
        uart_puts("Pi 4 Model B Rev 1.4 2GB RAM (Sony, UK)");
        break;
    default:
        uart_puts("Unknown board revision");
    }
}

/**
 * Format the mac address in hexadecimal format 00:00:00:00:00:00
 */
void uart_mac_address_hex(unsigned int address1, unsigned int address2)
{
    // little endian -> 4 bytes from 1st address in reverse order
    // , then 2 bytes from 2nd address in reverse order
    int cnt = 0;
    for (int pos = 0; pos < 32; pos += 8) // Start from the least significant byte
    {
        // Extract the byte at position pos
        char digit = (address1 >> pos) & 0xFF;
        // Convert digit to ASCII
        char digit1 = (digit >> 4) & 0xF;
        char digit2 = digit & 0xF;
        // Convert to ASCII code
        digit1 += (digit1 > 9) ? (-10 + 'A') : '0';
        digit2 += (digit2 > 9) ? (-10 + 'A') : '0';
        // Send each character individually
        uart_sendc(digit1);
        uart_sendc(digit2);
        cnt++; // Count number of pair
        uart_sendc(':');
    }
    for (int pos = 0; pos < 16; pos += 8) // Start from the least significant byte
    {
        // Extract the byte at position pos
        char digit = (address2 >> pos) & 0xFF;
        // Convert digit to ASCII
        char digit1 = (digit >> 4) & 0xF;
        char digit2 = digit & 0xF;
        // Convert to ASCII code
        digit1 += (digit1 > 9) ? (-10 + 'A') : '0';
        digit2 += (digit2 > 9) ? (-10 + 'A') : '0';
        // Send each character individually
        uart_sendc(digit1);
        uart_sendc(digit2);
        cnt++;
        if (cnt < 6) // 6 pairs -> 5 ':'
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

/**
 * Display welcome screen
 */
void uart_display_welcome_screen()
{
    uart_puts("\roooooooooooo oooooooooooo oooooooooooo ooooooooooooo   .oooo.         .o    .ooooo.     .oooo.   \n");
    uart_puts("\r`888'     `8 `888'     `8 `888'     `8 8'   888   `8 .dP  Y88b      .d88   888' `Y88.  d8P'`Y8b  \n");
    uart_puts("\r 888          888          888              888            ]8P'   .d'888   888    888 888    888 \n");
    uart_puts("\r 888oooo8     888oooo8     888oooo8         888          .d8P'  .d'  888    `Vbood888 888    888 \n");
    uart_puts("\r 888          888          888              888        .dP'     88ooo888oo       888' 888    888 \n");
    uart_puts("\r 888       o  888       o  888       o      888      .oP     .o      888       .88P'  `88b  d88' \n");
    uart_puts("\ro888ooooood8 o888ooooood8 o888ooooood8     o888o     8888888888     o888o    .oP'      `Y8bd8P'  \n");
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("\n");
    uart_puts("\r  .oooooo.   ooooo          .oooooo.   ooooo     ooo oooooooooo.        .oooooo.    .oooooo..o   \n");
    uart_puts("\r d8P'  `Y8b  `888'         d8P'  `Y8b  `888'     `8' `888'   `Y8b      d8P'  `Y8b  d8P'    `Y8   \n");
    uart_puts("\r888           888         888      888  888       8   888      888    888      888 Y88bo.        \n");
    uart_puts("\r888           888         888      888  888       8   888      888    888      888  `Y8888o.     \n");
    uart_puts("\r888           888         888      888  888       8   888      888    888      888      `Y88b    \n");
    uart_puts("\r`88b    ooo   888       o `88b    d88'  `88.    .8'   888     d88'    `88b    d88' oo     .d8P   \n");
    uart_puts("\r `Y8bood8P'  o888ooooood8  `Y8bood8P'     `YbodP'    o888bood8P'       `Y8bood8P'  8\"\"88888P'  \n");
    uart_puts("\n");
    uart_puts("\r    Developed by Le Tran Minh Trung - s3927071\n");
    uart_puts("\n");
    uart_puts("\rType help to show brief information of all commands\n");
    uart_puts("\n");
}

/**
 * Clear the screen and bring command to the top
 */
void uart_clear_screen()
{
    uart_puts("\033[2J\033[1;1H");
}

/**
 * Display os name
 */
void uart_display_os()
{
    uart_sendc('\r');
    uart_puts("CloudOS> ");
}

/**
 * clear line and then uart puts
 */
void uart_puts_clear_line(char *s)
{
    // Clear current line
    uart_sendc('\r');
    uart_sendc('\033');
    uart_sendc('[');
    uart_sendc('K');

    // Display OS name
    uart_display_os();

    // Display command
    uart_puts(s);
}

/**
 * Display list of available commands
 */
void uart_display_command_list()
{
    uart_puts("\rList of available commands:\n");
    uart_puts("\rclear, setcolor, showinfo\n");
    uart_puts("\ruart baudrate, uart databits, uart stopbits, uart parity, uart handshaking\n");
    uart_puts("\n");
    uart_puts("For more information on a specific command, type help command-name\n");
}

/**
 * Display detail description of a command
 */
void uart_display_command_detail(int command)
{
    // Display detail of command based on command number
    switch (command)
    {
    case 2:
        uart_puts("\rclear              clear the screen and bring the command line on top of the terminal\n");
        break;
    case 3:
        uart_puts("\rsetcolor           set text color, and/or background color of the console to one of the following\n\t\t\tcolors: black, red, green, yellow, blue, purple, cyan, white\n");
        uart_puts("\rsetcolor -t <color> -b <color>\n");
        break;
    case 4:
        uart_puts("\rshowinfo           show board revision and board MAC address in correct format/ meaningful information\n");
        break;
    case 5:
        uart_puts("\ruart baudrate      select a baudrate to configure for UART communication (9600, 19200, 38400, 57600, 115200)\n");
        break;
    case 6:
        uart_puts("\ruart databits      select a number to configure the number of data bits for UART communication (5, 6, 7, 8)\n");
        break;
    case 7:
        uart_puts("\ruart stopbits      select a number to configure the number of stop bits for UART communication (1, 2)\n");
        break;
    case 8:
        uart_puts("\ruart parity        configure UART parity settings (n - none, e - even, o - odd)\n");
        break;
    case 9:
        uart_puts("\ruart handshaking   configure UART handshaking settings.\n");
        break;
    }
}

/**
 * Set text color
 */
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

/**
 * Set background color
 */
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
    uart_puts("\e[K"); // Apply color to entire line
}