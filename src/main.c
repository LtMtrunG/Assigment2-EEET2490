// -----------------------------------main.c -------------------------------------
#include "mbox.h"
#include "uart.h"
#include "promptProcess.h"

void main()
{
    // set up serial console
    uart_init();
    // say hello
    uart_clear_screen();
    uart_display_welcome_screen();
    uart_display_os();

    // mBuf[0] = 16 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    // mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    // mBuf[2] = 0x00000001;   // TAG Identifier: Get clock rate
    // mBuf[3] = 4;            // Value buffer size in bytes (max of request and response lengths)
    // mBuf[4] = 0;            // REQUEST CODE = 0
    // mBuf[5] = 0;            // clock id: ARM system clock
    // mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])

    // mBuf[7] = 0x00010001;   // TAG Identifier: Get clock rate
    // mBuf[8] = 4;            // Value buffer size in bytes (max of request and response lengths)
    // mBuf[9] = 0;            // REQUEST CODE = 0
    // mBuf[10] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])

    char buffer[100]; // Buffer to store the input line
    int index = 0;    // Index to keep track of the current position in the buffer
    char previous_key = '\0';
    int result = -1;

    while (1)
    {
        // read each char
        char c = uart_getc();
        uart_sendc(c);

        if (c == '\n') // Check if Enter key is pressed
        {
            if (result != -1)
            {
                index = 0;
                while (options[result][index] != '\0')
                {
                    buffer[index] = options[result][index];
                    index++;
                }
            }
            buffer[index] = '\0'; // Null-terminate the string
            int num_of_words = total_words(buffer);
            if (is_help(buffer))
            {
                if (index == 4 && num_of_words == 1)
                {
                    uart_puts("List of available commands:\n");
                    uart_puts("clear\n");
                    uart_puts("setcolor\n   -t <text color>\n   -t <background color>\n");
                    uart_puts("showinfo\n");
                    uart_puts("\n");
                    uart_puts("For more information on a specific command, type help command-name\n");
                }
                else if (num_of_words == 2)
                {
                    int help_command = is_help_command(buffer);
                    if (help_command == 1)
                    {
                        uart_puts("clear            clear the screen and bring the command line on top of the terminal\n");
                    }
                    else if (help_command == 2)
                    {
                        uart_puts("setcolor         set text color, and/or background color of the console to one of the following colors: black, red, green, yellow, blue, purple, cyan, white\n");
                    }
                    else if (help_command == 3)
                    {
                        uart_puts("showinfo         show board revision and board MAC address in correct format/ meaningful information\n");
                    }
                    else
                    {
                        uart_puts("Invalid command! Type help to see available commands\n");
                    }
                }
                else
                {
                    uart_puts("Invalid command! Type help to see available commands\n");
                }
            }
            else if (is_setcolor(buffer))
            {
                int set_t_color = 0;
                int set_b_color = 0;
            }
            else if (is_clear(buffer))
            {
                uart_clear_screen();
            }
            else
            {
                uart_puts("Invalid command! Type help to see available commands\n");
            }
            for (int i = 0; i <= index; i++) // Clear the buffer
            {
                buffer[i] = '\0';
            }
            index = 0;
            uart_display_os();
        }
        else if ((c == 8 || c == 127) && index > 0)
        { // Handle delete/backspace character
            if (result != -1)
            {
                index = 0;
                while (options[result][index] != '\0')
                {
                    buffer[index] = options[result][index];
                    index++;
                }
            }
            result = -1;
            buffer[--index] = '\0';
            uart_puts("\b \b");
            // uart_sendc(index + '0');
        }
        else if (c == 9)
        {
            if (c != previous_key)
            {
                result = 0;
            }
            // uart_sendc(result);
            result = auto_complete(buffer, result);
            index = 0;
            while (options[result][index] != '\0')
            {
                index++;
            }
            uart_puts_clear_line(options[result]);
        }
        else if (c != 8 && c != 127)
        {
            result = -1;
            buffer[index++] = c;
        } // Store the character in the buffer
        previous_key = c;
    }
}

void get_firmware_revision()
{
    // mailbox data buffer: Read ARM frequency
    mBuf[0] = 8 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    mBuf[2] = 0x00000001;   // TAG Identifier: Get clock rate
    mBuf[3] = 4;            // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0;            // REQUEST CODE = 0
    mBuf[5] = 0;            // clock id: ARM system clock
    mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])
    mBuf[7] = MBOX_TAG_LAST;
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        uart_puts("Response Code for whole message: ");
        uart_hex(mBuf[1]);
        uart_puts("\n");
        uart_puts("Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\n");
        uart_puts("DATA: Firmware revision = ");
        uart_dec(mBuf[6]);
        uart_puts("\n");
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query!\n");
    }
}

void get_board_model()
{
    // mailbox data buffer: Read ARM frequency
    mBuf[0] = 8 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    mBuf[2] = 0x00010001;   // TAG Identifier: Get clock rate
    mBuf[3] = 4;            // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0;            // REQUEST CODE = 0
    mBuf[5] = 0;            // clock id: ARM system clock
    mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])
    mBuf[7] = MBOX_TAG_LAST;
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        uart_puts("Response Code for whole message: ");
        uart_hex(mBuf[1]);
        uart_puts("\n");
        uart_puts("Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\n");
        uart_puts("DATA: Board model = ");
        uart_dec(mBuf[6]);
        uart_puts("\n");
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query!\n");
    }
}

void get_board_revision()
{
    // mailbox data buffer: Read ARM frequency
    mBuf[0] = 8 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    mBuf[2] = 0x00010002;   // TAG Identifier: Get clock rate
    mBuf[3] = 4;            // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0;            // REQUEST CODE = 0
    mBuf[5] = 0;            // clock id: ARM system clock
    mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])
    mBuf[7] = MBOX_TAG_LAST;
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        uart_puts("Response Code for whole message: ");
        uart_hex(mBuf[1]);
        uart_puts("\n");
        uart_puts("Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\n");
        uart_puts("DATA: Board revision = ");
        uart_hex(mBuf[6]);
        uart_puts("\n");
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query!\n");
    }
}

void get_board_MACAddress()
{
    // mailbox data buffer: Read ARM frequency
    mBuf[0] = 8 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    mBuf[2] = 0x00010003;   // TAG Identifier: Get clock rate
    mBuf[3] = 6;            // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0;            // REQUEST CODE = 0
    mBuf[5] = 0;            // clock id: ARM system clock
    mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])
    mBuf[7] = MBOX_TAG_LAST;
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        uart_puts("Response Code for whole message: ");
        uart_hex(mBuf[1]);
        uart_puts("\n");
        uart_puts("Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\n");
        uart_puts("DATA: Board MAC address = ");
        uart_hex(mBuf[6]);
        uart_puts("\n");
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query!\n");
    }
}

void get_board_serial()
{
    // mailbox data buffer: Read ARM frequency
    mBuf[0] = 8 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    mBuf[2] = 0x00010004;   // TAG Identifier: Get clock rate
    mBuf[3] = 6;            // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0;            // REQUEST CODE = 0
    mBuf[5] = 0;            // clock id: ARM system clock
    mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])
    mBuf[7] = MBOX_TAG_LAST;
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        uart_puts("Response Code for whole message: ");
        uart_hex(mBuf[1]);
        uart_puts("\n");
        uart_puts("Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\n");
        uart_puts("DATA: Board serial = ");
        uart_dec(mBuf[6]);
        uart_puts("\n");
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query!\n");
    }
}

void get_clockrate()
{
    // mailbox data buffer: Read ARM frequency
    mBuf[0] = 8 * 4;        // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
    mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)
    mBuf[2] = 0x00030002;   // TAG Identifier: Get clock rate
    mBuf[3] = 8;            // Value buffer size in bytes (max of request and response lengths)
    mBuf[4] = 0;            // REQUEST CODE = 0
    mBuf[5] = 3;            // clock id: ARM system clock
    mBuf[6] = 0;            // clear output buffer (response data are mBuf[5] & mBuf[6])
    mBuf[7] = MBOX_TAG_LAST;
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        uart_puts("Response Code for whole message: ");
        uart_hex(mBuf[1]);
        uart_puts("\n");
        uart_puts("Response Code in Message TAG: ");
        uart_hex(mBuf[4]);
        uart_puts("\n");
        uart_puts("DATA: ARM clock rate = ");
        uart_dec(mBuf[6]);
        uart_puts("\n");
        uart_puts("\n");
    }
    else
    {
        uart_puts("Unable to query!\n");
    }
}