// -----------------------------------main.c -------------------------------------
#include "mbox.h"
#include "uart.h"
#include "promptProcess.h"

#define MAX_LENGTH 100
int uart_t_color = 5;
int uart_b_color = 7;
int baudrate = 115200;
int data_bits = 8;
int stop_bits = 1;
char parity = 'n';

int main()
{
    // set up serial console
    uart_init();
    uart_set_t_color();
    uart_set_b_color();
    // say hello
    uart_clear_screen();
    uart_display_welcome_screen();
    uart_display_os();

    char buffer[MAX_LENGTH]; // Buffer to store the input line
    int index = 0;           // Index to keep track of the current position in the buffer
    char previous_key = '\0';
    int result = -1;
    int flag = 0;
    int is_valid_command = 1;
    char command[MAX_LENGTH][MAX_LENGTH];
    int command_list_size = 0;

    // Initialize command history list array
    for (int i = 0; i < MAX_LENGTH; i++)
    {
        for (int j = 0; j < MAX_LENGTH; j++)
        {

            command[i][j] = '\0'; // Ensure each string is initially empty
        }
        buffer[i] = '\0';
    }

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
                // Copy the command selected from command's history list to buffer
                if (previous_key == '+' || previous_key == '_') 
                {
                    index = copy_command(buffer, command[result]);
                }
                else // Copy the command selected by auto-complete from command list to buffer
                {
                    index = copy_command(buffer, options[result]);
                }
                result = -1;
            }
            int help_command = is_help(buffer); // Get the matching result with help (help/help-command)
            if (help_command) // If the command is valid
            {
                if (help_command == 1) // Display available list (help)
                    uart_display_command_list();
                else // Display command's detail (help command-name)
                {
                    uart_display_command_detail(help_command);
                }
            }
            else if (is_setcolor(buffer)) // Check if the user type setcolor
            {
                if (check_format(buffer)) // Check user's format 
                {
                    if (prompt_b_color != -1) // If user input valid background color -> change color
                    {
                        uart_b_color = prompt_b_color;
                        uart_set_b_color();
                    }
                    if (prompt_t_color != -1) // If user input valid text color -> change color
                    {
                        uart_t_color = prompt_t_color;
                        uart_set_t_color();
                    }
                }
                else // Wrong format -> Error
                {
                    uart_puts("\rWrong format! Type help setcolor to see more information\n");
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
                    uart_puts("\rBoard Revision = ");
                    uart_print_board_revision(mBuf[5]);
    
                    uart_puts("\n\rBoard MAC Address = ");
                    uart_mac_address_hex(mBuf[9], mBuf[10]);
                    uart_puts("\n");
                }
                else
                {
                    uart_puts("\rUnable to query!\n");
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
                    is_valid_command = 0;
                }
            }
            else if (is_command(buffer, "uart databits"))
            {
                uart_puts("Select a number to config (5, 6, 7, 8)\n");
                uart_puts("Enter your choice: ");
                flag = 2;
            }
            else if (flag == 2)
            {
                if (uart_set_data_bits(buffer))
                {
                    flag = 0;
                    is_valid_command = 0;
                }
            }
            else if (is_command(buffer, "uart stopbits"))
            {
                uart_puts("Select a number to config (1, 2)\n");
                uart_puts("Enter your choice: ");
                flag = 3;
            }
            else if (flag == 3)
            {
                if (uart_set_stop_bits(buffer))
                {
                    flag = 0;
                    is_valid_command = 0;
                }
            }
            else if (is_command(buffer, "uart parity"))
            {
                uart_puts("Select an option to config (n - none, e - even, o - odd)\n");
                uart_puts("Enter your choice: ");
                flag = 4;
            }
            else if (flag == 4)
            {
                if (uart_set_parity(buffer))
                {
                    flag = 0;
                    is_valid_command = 0;
                }
            }
            else if (is_command(buffer, "uart handshaking"))
            {
                uart_puts("Select an option to config (c - CTS, r - RTS)\n");
                uart_puts("Enter your choice: ");
                flag = 5;
            }
            else if (flag == 5)
            {
                if (uart_set_handshaking(buffer))
                {
                    flag = 0;
                    is_valid_command = 0;
                }
            }
            else
            {
                uart_puts("\rInvalid command! Type help to see available commands\n");
                is_valid_command = 0;
            }
            if (is_valid_command == 1)
            {
                copy_command(command[command_list_size++], buffer);
            }
            for (int i = 0; i < index; i++) // Clear the buffer
            {
                buffer[i] = '\0';
            }
            index = 0;
            is_valid_command = 1;
            if (!flag)
            {
                uart_display_os();
            }
        }
        else if ((c == 8 || c == 127) && index > 0) // Handle delete/backspace character
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
            result = -1;
            buffer[--index] = '\0';
            uart_puts("\b \b");
        }
        else if (c == 9) // Handle tab button
        {
            if (c != previous_key) // Not tab before
            {
                result = 0; // Set result to 0 to start searching from zero
            }
            result = auto_complete(buffer, result); // Find position of matching command
            if (result != -1)
            {
                uart_puts_clear_line(options[result]); // Display matching command
            }
            else
            {
                uart_puts("\nNo matching commands! Please type help to see available commands\n");
                while (index > 0)
                {
                    buffer[--index] = '\0'; // Clear buffer
                }
                uart_display_os();
            }
        }
        else if (c == '+') // Handle arrow up
        {
            if (previous_key == '+' || previous_key == '_') // Continue up
            {
                if (result > 0) // If up not reach 0 -> decrease
                {
                    result--;
                }
                else // Reach 0 -> Stay at 0
                {
                    result = 0;
                }
            }
            else // 1st time up -> start at lastest command
            {
                result = command_list_size - 1;
            }
            uart_puts_clear_line(command[result]);
        }
        else if (c == '_') // Handle arrow down
        {
            if (previous_key == '+' || previous_key == '-') // Continue down
            {
                if (result < command_list_size - 1) // If up not reach lastest command -> up
                {
                    result++;
                }
                else // Reach lastest command -> Stay at lastest
                {
                    result = command_list_size - 1;
                }
            }
            else // 1st time down -> start at lastest command
            {
                result = command_list_size - 1;
            }
            uart_puts_clear_line(command[result]);
        }
        else //Any other characters 
        {
            if (result != -1) {
                index = copy_command(buffer, options[result]);
                result = -1;
            }
            buffer[index++] = c;
        } // Store the character in the buffer
        previous_key = c;
    }
    return 1;
}
