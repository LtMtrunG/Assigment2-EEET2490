// -----------------------------------promptProcess.c -------------------------------------
#include "promptProcess.h"

// List of available options
char options[][20] = {"uart baudrate", "help", "help clear", "help showinfo", "help setcolor", "clear", "showinfo", "setcolor"};
int num_options = sizeof(options) / sizeof(options[0]);
int prompt_b_color, prompt_t_color;

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
    char words_list[5][100];
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            words_list[i][j] = '\0';
        }
    }

    prompt_b_color = -1;
    prompt_t_color = -1;

    int index = 0;
    int jindex = 0;
    while (*prompt != '\0')
    {
        if ((*prompt >= 'a' && *prompt <= 'z') || *prompt == '-')
        {
            words_list[index][jindex++] = *prompt;
        }
        else if (*prompt == ' ' && ((*(prompt + 1) >= 'a' && *(prompt + 1) <= 'z') || *(prompt + 1) == '-'))
        {
            index++;
            jindex = 0;
        }
        else
        {
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }
        prompt++;
    }
    index++;

    if (index == 2 || index == 4)
    {
        int color = get_color(words_list[1]);
        if (color == -1)
        {
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }
        if (is_t(words_list[0]))
        {
            prompt_t_color = color;
            prompt_b_color = -1;
        }
        else if (is_b(words_list[0]))
        {
            prompt_b_color = color;
            prompt_t_color = -1;
        }
        else
        {
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }

        if (index == 2)
        {
            return 1;
        }
    }

    if (index == 4)
    {
        int color = get_color(words_list[3]);
        if (color == -1)
        {
            prompt_b_color = -1;
            prompt_t_color = -1;
            return 0;
        }
        if (is_t(words_list[2]) && prompt_t_color == -1)
        {
            prompt_t_color = color;
            return 1;
        }
        else if (is_b(words_list[2]) && prompt_b_color == -1)
        {
            prompt_b_color = color;
            return 1;
        }

        prompt_b_color = -1;
        prompt_t_color = -1;
        return 0;
    }

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
        if (temp == num_options)
        {
            temp = 0;
        }
        if (temp == index) {
            return -1;
        }
        if (is_command(prompt, options[temp]))
        {
            return temp;
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