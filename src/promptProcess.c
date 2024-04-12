#include "promptProcess.h"

// List of available options
char options[][20] = {"help", "help clear", "help showinfo", "help setcolor", "clear", "showinfo", "setcolor"};
int num_options = sizeof(options) / sizeof(options[0]);

int is_help(char *prompt)
{
    const char *command = "help";
    while (*prompt != '\0' && *command != '\0')
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

int is_help_command(char *prompt)
{
    prompt += 5; // Move the pointer to skip the "help " prefix

    // Check if the command is "clear"
    if (is_command(prompt, "clear"))
    {
        return 1;
    }

    // Check if the command is "setcolor"
    if (is_command(prompt, "setcolor"))
    {
        return 2;
    }

    // Check if the command is "showinfo"
    if (is_command(prompt, "showinfo"))
    {
        return 3;
    }

    return 0; // Not a recognized command
}

int is_command(char *prompt, const char *command)
{
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0; // Characters don't match
        }
        prompt++;
        command++;
    }
    if (*prompt != '\0')
    {
        return 0; // Command matches and prompt is followed by null terminator or space
    }
    return 1; // Command doesn't match or prompt continues
}

int is_setcolor(char *prompt)
{
    const char *command = "setcolor";
    while (*prompt != '\0' && *command != '\0')
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

int check_format(char *prompt)
{
    prompt += 9;
    char words_list[100][100];
    int index = 0, jindex = 0;
    int set_t = 0, set_b =0; 
    while (*prompt != '\0')
    {
        if (*prompt >= 'a' && *prompt <= 'z')
        {
            words_list[index][jindex] = *prompt;
            prompt++;
            jindex++;
        } else if (*prompt == ' ' && *(prompt + 1) >= 'a' && *(prompt + 1) <= 'z') {
            index++;
            jindex=0;
        } else {
            return 0;
        }
    }
    if (index == 2) {
        if (is_t(words_list[0])) {
            set_t++;
        } else if (is_b(words_list[0])) {
            set_b++;
        } else {
            return 0;
        } 
        if (!is_color(words_list[1])) {
            return 0;
        } 
    }
}

int is_t(char *prompt) 
{
    const char *command = "-t";
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0;
        }
        prompt++;
        command++;
    }
    if (*prompt != '\0')
    {
        return 0;
    }
    return 1;
}

int is_b(char *prompt) 
{
    const char *command = "-b";
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0;
        }
        prompt++;
        command++;
    }
    if (*prompt != '\0')
    {
        return 0;
    }
    return 1;
}

int is_color(char *prompt, const char *command)
{
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0; // Characters don't match
        }
        prompt++;
        command++;
    }
    if (*prompt != '\0')
    {
        return 0; // Command matches and prompt is followed by null terminator or space
    }
}

int is_clear(char *prompt)
{
    const char *command = "clear";
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0;
        }
        prompt++;
        command++;
    }
    if (*prompt != '\0')
    {
        return 0;
    }
    return 1;
}

int auto_complete(char *prompt, int index)
{
    while (1)
    {
        index++;
        if (index == num_options)
        {
            index = 0;
        }
        if (is_command_match(prompt, options[index]))
        {
            return index;
        }
    }
}

int is_command_match(char *prompt, char *command)
{
    while (*prompt != '\0' && *command != '\0')
    {
        if (*prompt != *command)
        {
            return 0;
        }
        prompt++;
        command++;
    }
    if (*prompt != '\0')
    {
        return 0;
    }
    return 1;
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