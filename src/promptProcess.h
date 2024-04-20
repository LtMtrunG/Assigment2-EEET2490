int is_help(char *prompt);
int is_setcolor(char *prompt);
int auto_complete(char *prompt, int index);
int total_words(char *prompt);
int is_help_command(char *prompt);
int is_command(char *prompt, const char *command);
int check_format(char *prompt);
int is_t(char *prompt) ;
int is_b(char *prompt);
int is_color(char *prompt, const char *color);
int get_color(char *prompt);

#ifndef PROMPTPROCESS_H
#define PROMPTPROCESS_H

extern char options[][20];
extern int num_options;
extern int prompt_t_color;
extern int prompt_b_color;

#endif 



