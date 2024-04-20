// -----------------------------------main.c -------------------------------------
#include "mbox.h"
#include "uart.h"
#include "promptProcess.h"

#define MAX_LENGTH 100
int uart_t_color = 5;
int uart_b_color = 7;

int main()
{
    // set up serial console
    uart_init(115200, 8);
    uart_set_t_color();
    uart_set_b_color();
    // say hello
    uart_clear_screen();
    uart_display_welcome_screen();
    uart_display_os();

    char buffer[100]; // Buffer to store the input line
    int index = 0;    // Index to keep track of the current position in the buffer
    char previous_key = '\0';
    int result = -1;
    int flag = 0;

    while (1)
    {
        // read each char
        char c = uart_getc();
        // uart_set_b_color();

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
                if (check_format(buffer))
                {
                    if (prompt_b_color != -1)
                    {
                        uart_b_color = prompt_b_color;
                        uart_set_b_color();
                    }
                    if (prompt_t_color != -1)
                    {
                        uart_t_color = prompt_t_color;
                        uart_set_t_color();
                    }
                }
                else
                {
                    uart_puts("Wrong format! Type help setcolor to see more information\n");
                }
            }
            else if (is_command(buffer, "clear"))
            {
                uart_clear_screen();
            }
            else if (is_command(buffer, "showinfo"))
            {
                // mailbox data buffer: Read ARM frequency
                mBuf[0] = 12 * 4;       // Message Buffer Size in bytes (8 elements * 4 bytes (32 bit) each)
                mBuf[1] = MBOX_REQUEST; // Message Request Code (this is a request message)

                mBuf[2] = 0x00010002; // TAG Identifier: Get clock rate
                mBuf[3] = 4;          // Value buffer size in bytes (max of request and response lengths)
                mBuf[4] = 0;          // REQUEST CODE = 0
                mBuf[5] = 0;          // clear output buffer (response data are mBuf[5])

                mBuf[6] = 0x00010003; // TAG Identifier: Get firmware revision
                mBuf[7] = 6;          // Value buffer size in bytes (max of request and response lengths)
                mBuf[8] = 0;          // REQUEST CODE = 0
                mBuf[9] = 0;          // clear output buffer (response data are mBuf[9])
                mBuf[10] = 0;         // clear output buffer (response data are mBuf[9])

                mBuf[11] = MBOX_TAG_LAST;
                if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
                {
                    uart_puts("\nResponse Code for whole message: ");
                    uart_hex(mBuf[1]);

                    uart_puts("\n+ Response Code in Message TAG: ");
                    uart_hex(mBuf[4]);
                    uart_puts("\nDATA: Board Revision = ");
                    uart_hex(mBuf[5]);

                    uart_puts("\n+ Response Code in Message TAG: ");
                    uart_hex(mBuf[8]);
                    uart_puts("\nDATA: Board MAC Address = ");
                    uart_mac_address_hex(mBuf[9], mBuf[10]);

                    uart_puts("\n");
                }
                else
                {
                    uart_puts("Unable to query!\n");
                }
            }
            else if (is_command(buffer, "uart baudrate"))
            {
                uart_puts("Select a baudrate to config\n");
                uart_puts("1. 9600\n");
                uart_puts("2. 19200\n");
                uart_puts("3. 38400\n");
                uart_puts("4. 57600\n");
                uart_puts("5. 115200\n");
                uart_puts("Enter your choice: ");
                flag = 1;
            }
            else if (flag == 1)
            {
                if (uart_set_baudrate(buffer))
                {
                    flag = 0;
                }
            }
            // else if (is_command(buffer, "uart databits"))
            // {
            //     uart_puts("Select a number to config (5, 6, 7, 8)\n");
            //     uart_puts("Enter your choice: ");
            //     flag = 2;
            // }
            // else if (flag == 2)
            // {
            //     if (uart_set_data_bits(buffer))
            //     {
            //         flag = 0;
            //     }
            // }
            else
            {
                uart_puts("Invalid command! Type help to see available commands\n");
            }
            for (int i = 0; i <= index; i++) // Clear the buffer
            {
                buffer[i] = '\0';
            }
            index = 0;
            if (!flag)
            {
                uart_display_os();
            }
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
        }
        else if (c == 9)
        {
            if (c != previous_key)
            {
                result = 0;
            }
            result = auto_complete(buffer, result);
            if (result != -1)
            {
                index = 0;
                while (options[result][index] != '\0')
                {
                    index++;
                }
                uart_puts_clear_line(options[result]);
            }
            else
            {
                uart_puts("\nNo matching commands! Please type help to see available commands\n");
                while (index > 0)
                {
                    buffer[--index] = '\0';
                }
                uart_display_os();
            }
        }
        else if (c != 8 && c != 127)
        {
            result = -1;
            buffer[index++] = c;
        } // Store the character in the buffer
        previous_key = c;
    }
    return 1;
}
