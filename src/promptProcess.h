int is_help(char *prompt);
int is_clear(char *prompt);
int is_setcolor(char *prompt);
int auto_complete(char *prompt, int index);
int is_command_match(char *prompt, char *command);
int total_words(char *prompt);
int is_help_command(char *prompt);
int is_command(char *prompt, const char *command);
// options.h

#ifndef OPTIONS_H
#define OPTIONS_H

extern char options[][20];
extern int num_options;

#endif /* OPTIONS_H */



