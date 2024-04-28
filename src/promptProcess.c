// -----------------------------------promptProcess.c -------------------------------------
#include "promptProcess.h"

// List of available options
char options[][30] = {"help", "help clear", "help showinfo", "help setcolor", "help uart baudrate", "help uart databits", "help uart stopbits", "help uart parity", "help uart handshaking", "clear", "showinfo", "setcolor", "uart baudrate", "uart databits", "uart stopbits", "uart parity", "uart handshaking"};
int num_options = sizeof(options) / sizeof(options[0]);
int prompt_b_color, prompt_t_color;

int is_help(char *prompt)
{
    const char *command = "help";
    // Run through both prompt and command until one of them is end
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command) // If the prompt and the command are not match
        {
            return 0;
        }
        // Increase prompt and command
        prompt++;
        command++;
    }
    // If both prompt and command is end -> help
    if (*prompt == '\0' && *command == '\0')
    {
        return 1;
    }
    // Else check for help command-name or invalid help something
    return is_help_command(++prompt);
}

int is_help_command(char *prompt)
{
    // Check if the command is match with any system's command

    if (is_command(prompt, "clear"))
    {
        return 2;
    }

    if (is_command(prompt, "setcolor"))
    {
        return 3;
    }

    if (is_command(prompt, "showinfo"))
    {
        return 4;
    }

    if (is_command(prompt, "uart baudrate"))
    {
        return 5;
    }

    if (is_command(prompt, "uart databits"))
    {
        return 6;
    }

    if (is_command(prompt, "uart stopbits"))
    {
        return 7;
    }

    if (is_command(prompt, "uart parity"))
    {
        return 8;
    }

    if (is_command(prompt, "uart handshaking"))
    {
        return 9;
    }

    return 0; // Not a recognized command
}

/**
 * Check if two strings are the same
 */
int is_command(char *prompt, const char *command)
{
    // Run through both prompt and command until both of them is end
    while (*prompt != '\0' || *command != '\0')
    {
        if (*prompt != *command) // Characters don't match -> return false
        {
            return 0;
        }
        // Increase prompt and command
        prompt++;
        command++;
    }
    return 1; // Return true
}

/**
 * Check if prompt is part of command (index by index, left to right)
 */
int is_match(char *prompt, const char *command)
{
    // Run through both prompt and command until one of them is end
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command) // If the prompt and the command are not matched -> return false
        {
            return 0;
        }
        // Increase prompt and command
        prompt++;
        command++;
    }
    if (*prompt != '\0') // If prompt contain extra characters -> Return false
    {
        return 0;
    }
    return 1; // Return true
}

int is_setcolor(char *prompt)
{
    const char *command = "setcolor";
    // Run through both prompt and command until one of them is end
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command) // If the prompt and the command are not match
        {
            return 0;
        }
        // Increase prompt and command
        prompt++;
        command++;
    }
    // If both prompt and command is end -> help
    if (*prompt == '\0' && *command == '\0')
    {
        return 1;
    }
    // The first word is matched with setcolor
    return 1;
}

int check_format(char *prompt)
{
    prompt += 9; // Skip "setcolor "

    // Array to store words after "setcolor "
    char words_list[5][100];
    for (int i = 0; i < 5; i++) // Initialize
    {
        for (int j = 0; j < 100; j++)
        {
            words_list[i][j] = '\0';
        }
    }

    // Default value for user's color prompt
    prompt_b_color = -1;
    prompt_t_color = -1;

    int index = 0;
    int jindex = 0;
    while (*prompt != '\0') // Loop through the prompt
    {
        // Store character from a->z and -
        if ((*prompt >= 'a' && *prompt <= 'z') || *prompt == '-')
        {
            words_list[index][jindex++] = *prompt;
        }
        // If the current character is a space and next character is a->z or - -> Increase index to store new word
        else if (*prompt == ' ' && ((*(prompt + 1) >= 'a' && *(prompt + 1) <= 'z') || *(prompt + 1) == '-'))
        {
            index++;
            jindex = 0;
        }
        // Invalid characters or multiple black space
        else
        {
            // Set invalid value for both color, then return false
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }
        prompt++; // Increase prompt
    }
    index++;

    // If the number of words is 2 and 4
    if (index == 2 || index == 4)
    {
        int color = get_color(words_list[1]); // Extract color from user's prompt
        if (color == -1)                      // Invalid color
        {
            // Set invalid value for both color, then return false
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }
        if (is_t(words_list[0])) // Check if user want to set text color
        {
            prompt_t_color = color; // Set new value for prompt text color
            prompt_b_color = -1;    // Set invalid value prompt text color
        }
        else if (is_b(words_list[0])) // Check if user want to set background color
        {
            prompt_b_color = color; // Set new value for prompt background color
            prompt_t_color = -1;    // Set invalid value prompt background color
        }
        else // If user not input neither -t or -b
        {
            // Set invalid value for both color, then return false
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }

        // If there is only 2 words -> User only want to set text or background color
        if (index == 2)
        {
            return 1; // Return true
        }
    }

    // If the number of words is 4
    if (index == 4)
    {
        int color = get_color(words_list[3]); // Extract color from user's prompt
        if (color == -1)                      // Invalid color
        {
            // Set invalid value for both color, then return false
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }
        if (is_t(words_list[2]) && prompt_t_color == -1) // Check if user want to set text color
        {                                                // and text color is not set before
            prompt_t_color = color;                      // Set new value for prompt text color
            return 1;                                    // return true
        }
        else if (is_b(words_list[2]) && prompt_b_color == -1) // Check if user want to set background color
        {                                                     // and background color is not set before
            prompt_b_color = color;                           // Set new value for prompt background color
            return 1;                                         // return true
        }

        // If user not input neither -t or -b -> Set invalid value for both color, then return false
        prompt_b_color = -1;
        prompt_t_color = -1;
        return 0;
    }

    // If prompt does not match any case -> Set invalid value for both color, then return false
    prompt_b_color = -1;
    prompt_t_color = -1;
    return 0;
}

int is_t(char *prompt)
{
    const char *command = "-t";
    while (*prompt != '\0' || *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0;
        }
        prompt++;
        command++;
    }
    return 1;
}

int is_b(char *prompt)
{
    const char *command = "-b";
    while (*prompt != '\0' || *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0;
        }
        prompt++;
        command++;
    }
    return 1;
}

int get_color(char *prompt)
{
    if (is_color(prompt, "black"))
    {
        return 0;
    }
    if (is_color(prompt, "red"))
    {
        return 1;
    }
    if (is_color(prompt, "green"))
    {
        return 2;
    }
    if (is_color(prompt, "yellow"))
    {
        return 3;
    }
    if (is_color(prompt, "blue"))
    {
        return 4;
    }
    if (is_color(prompt, "purple"))
    {
        return 5;
    }
    if (is_color(prompt, "cyan"))
    {
        return 6;
    }
    if (is_color(prompt, "white"))
    {
        return 7;
    }
    return -1;
}

int is_color(char *prompt, const char *color)
{
    while (*prompt != '\0' || *color != '\0')
    {
        if (*prompt != *color)
        {
            return 0; // Characters don't match
        }
        prompt++;
        color++;
    }
    return 1;
}

int auto_complete(char *prompt, int index)
{
    int temp = index;
    while (1)
    {
        temp++;
        if (temp == num_options) // If the loop has go to the end of the array
        {                        
            temp = 0;   // Reset it back to zero to search cyclically
        }
        if (temp == index) // If the serch process go back to the search starting point
        {
            return -1;  // Return null since theres no matching result
        }
        if (is_match(prompt, options[temp]))
        {
            return temp; // Return the position of matching result
        }
    }
}

int total_words(char *prompt)
{
    int ans = 1;
    while (*prompt != '\0')
    {
        if (*prompt == ' ' && *(prompt + 1) >= 'a' && *(prompt + 1) <= 'z')
        {
            ans++;
        }
        prompt++;
    }
    return ans;
}

// Function to copy a string from source to destination
int copy_command(char destination[], const char source[])
{
    int i = 0;
    while (source[i] != '\0')
    {
        destination[i] = source[i];
        i++;
    }
    destination[i] = '\0'; // Add null terminator to the end of the destination string
    return i;
}