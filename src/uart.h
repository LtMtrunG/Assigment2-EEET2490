// -----------------------------------uart.h -------------------------------------
/* Auxilary mini UART (UART1) registers */
#define AUX_ENABLE      (* (volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       (* (volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      (* (volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      (* (volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      (* (volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      (* (volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      (* (volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      (* (volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  (* (volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     (* (volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     (* (volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     (* (volatile unsigned int*)(MMIO_BASE+0x00215068))

#define ANSI_COLOR_BG_BLACK   "\x1b[40m"
#define ANSI_COLOR_BG_RED     "\x1b[41m"
#define ANSI_COLOR_BG_GREEN   "\x1b[42m"
#define ANSI_COLOR_BG_YELLOW  "\x1b[43m"
#define ANSI_COLOR_BG_BLUE    "\x1b[44m"
#define ANSI_COLOR_BG_MAGENTA "\x1b[45m"
#define ANSI_COLOR_BG_CYAN    "\x1b[46m"
#define ANSI_COLOR_BG_WHITE   "\x1b[47m"

#define ANSI_COLOR_FG_BLACK   "\x1b[30m"
#define ANSI_COLOR_FG_RED     "\x1b[31m"
#define ANSI_COLOR_FG_GREEN   "\x1b[32m"
#define ANSI_COLOR_FG_YELLOW  "\x1b[33m"
#define ANSI_COLOR_FG_BLUE    "\x1b[34m"
#define ANSI_COLOR_FG_MAGENTA "\x1b[35m"
#define ANSI_COLOR_FG_CYAN    "\x1b[36m"
#define ANSI_COLOR_FG_WHITE   "\x1b[37m"

/* Function prototypes */
void uart_init();
void uart_sendc(char c);
char uart_getc();
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_dec(int num);
void uart_display_welcome_screen();
//void display_help();
void uart_clear_screen();
void uart_display_os();
//void uart_process_prompt(char *prompt, int size);
void uart_puts_clear_line(char *s);


