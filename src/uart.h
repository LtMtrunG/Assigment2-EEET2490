// -----------------------------------uart.h -------------------------------------
/* PL011 UART (UART0) registers */
#define UART0_BASE	(MMIO_BASE + 0x201000)

/* DR = Data register */
#define UART0_DR	(* (volatile unsigned int*)(UART0_BASE + 0x00))
/* On write, write 8-bits to send */
/* On receive, get 12-bits:       */
/*    Overrun Error == Overflowed FIFO */
/*    Break Error == data held low over full time */
/*    Parity Error */
/*    Frame Error == missing stop bit */
/* Then 8 bits of data */

/* RSRECR = Receive Status Register */
/*  Has same 4 bits of error as in DR */
#define UART0_RSRECR (* (volatile unsigned int*)(UART0_BASE + 0x04))

/* FR = Flags Register */
#define UART0_FR	(* (volatile unsigned int*)(UART0_BASE + 0x18))
#define UART0_FR_TXFE	(1<<7)	/* TXFE = Transmit FIFO empty */
#define UART0_FR_RXFF	(1<<6)	/* RXFF = Receive FIFO full   */
#define UART0_FR_TXFF	(1<<5)	/* TXFF = Transmit FIFO full */
#define UART0_FR_RXFE	(1<<4)	/* RXFE = Receive FIFO empty */
#define UART0_FR_BUSY	(1<<3)	/* BUSY = UART is busy transmitting */
				/*  2 = DCD  = Unsupported */
				/*  1 = DSR  = Unsupported */
#define UART0_FR_CTS	(1<<0)	/* CTS  = inverted version of nUARTCTS value */

/* ILPR = Infrared, disabled on BC2835 */
#define UART0_ILPR	(* (volatile unsigned int*)(UART0_BASE + 0x20))
/* IBRD = Integer part of Baud Rate Divisor */
/*    bottom 16 bits */
/*    UART must be disabled to change */
#define UART0_IBRD	(* (volatile unsigned int*)(UART0_BASE + 0x24))
/* FBRD = Fractional part of Baud Rate Divisor */
/*   bottom 5 bits */
/*   Baud rate divisor BAUDDIV = (FUARTCLK/(16 Baud rate)) */
/*    UART must be disabled to change */
#define UART0_FBRD	(* (volatile unsigned int*)(UART0_BASE + 0x28))
/* LCRH = Line Control Register */
#define UART0_LCRH	(* (volatile unsigned int*)(UART0_BASE + 0x2C))
#define UART0_LCRH_SPS	(1<<7)	/* SPS = Stick Parity Select (0=disabled) */
				/* WLEN = word length 00=5, 01=6, 10=7, 11=8 */
#define UART0_LCRH_WLEN_CLR		(3<<5)
#define UART0_LCRH_WLEN_5BIT	(0b00<<5)
#define UART0_LCRH_WLEN_6BIT	(0b01<<5)
#define UART0_LCRH_WLEN_7BIT	(0b10<<5)
#define UART0_LCRH_WLEN_8BIT	(0b11<<5)
#define UART0_LCRH_FEN	(1<<4)	/* FEN = enable FIFOs */
#define UART0_LCRH_CTP	(1<<3)	/* CTP  = clear stop bit */
#define UART0_LCRH_STP2	(1<<3)	/* STP2 = enable 2 stop bits */
#define UART0_LCRH_EPS	(1<<2)	/* EPS  = even parity select */
#define UART0_LCRH_PEN	(1<<1)	/* PEN  = parity enable */
#define UART0_LCRH_BRK	(1<<0)	/* BRK  = send break after next character */
/* CR = Control Register */
/*   To enable transmission TXE and UARTEN must be set to 1 */
/*   To enable reception RXE and UARTEN must be set to 1 */
/*   Program the control registers as follows: */
/*    1. Disable the UART.                     */
/*    2. Wait for the end of transmission or   */
/*       reception of the current character.   */
/*    3. Flush the transmit FIFO by setting    */
/*       the FEN bit to 0 in the Line Control  */
/*       Register, UART_LCRH.                  */
/*    4. Reprogram UART_CR                     */
/*    5. Enable the UART.                      */
#define UART0_CR	(* (volatile unsigned int*)(UART0_BASE + 0x30))
#define UART0_CR_CTSEN	(1<<15)	/*   15 = CTSEN = CTS Flow Enable */
#define UART0_CR_RTSEN	(1<<14)	/*   14 = RTSEN = RTS Flow Enable */
				/*   13 = OUT2 = Unsupported */
				/*   12 = OUT1 = Unsupported */
#define UART0_CR_RTS	(1<<11)	/*   11 = RTS = Request to Send */
					/*   10 = DTR = Unsupported */
#define UART0_CR_RXE	(1<<9)	/*    9 = RXE = Receive Enable */
#define UART0_CR_TXE	(1<<8)	/*    8 = TXE = Transmit Enable */
#define UART0_CR_LBE	(1<<7)	/*    7 = LBE = Loopback Enable */
				/*    6 - 3 = RESERVED */
				/*    2 = SIRLP = Unsupported */
				/*    1 = SIREN = Unsupported */
#define UART0_CR_UARTEN	(1<<0)	/*    0 = UARTEN = UART Enable */
/* IFLS = FIFO Level Select */
/*  11 - 9 = RXIFPSEL = Unsupported */
/*   8 - 6 = TXIFPSEL = Unsupported */
/*   5 - 3 = RXIFLSEL = 000=1/8, 001=1/4, 010=1/2, 011=3/4 100=7/8 */
/*   2 - 0 = TXIFLSEL = 000=1/8, 001=1/4, 010=1/2, 011=3/4 100=7/8 */
#define UART0_IFLS	(* (volatile unsigned int*)(UART0_BASE + 0x34))
/* IMSRC = Interrupt Mask Set/Clear */
/*   10 = OEIM = Overrun Interrupt Mask */
/*    9 = BEIM = Break Interrupt Mask */
/*    8 = PEIM = Parity Interrupt Mask */
/*    7 = FEIM = Framing Interrupt Mask */
/*    6 = RTIM = Receivce Timeout Mask */
/*    5 = TXIM = Transmit Interrupt Mask */
/*    4 = RXIM = Receive Masked Interrupt Mask */
/*    3 = DSRMIM (unsupported) */
/*    2 = DCDMIM (unsupported) */
/*    1 = CTSMIM = nUARTCTS Mask */
/*    0 = RIMIM (unsupported) */
#define UART0_IMSC	(* (volatile unsigned int*)(UART0_BASE + 0x38))
/* RIS = Raw Interrupt Status */
#define UART0_IMSC_OE	(1<<10) /* OERIS = Overrun Interrupt Raw Status */
#define UART0_IMSC_BE	(1<<9)  /* BERIS = Break Interrupt Raw Status*/
#define UART0_IMSC_PE	(1<<8)  /* PERIS = Parity Interrupt Raw Status */
#define UART0_IMSC_FE	(1<<7)	/* FERIS = Framing Interrupt Raw Status */
#define UART0_IMSC_RT	(1<<6)	/* RTRIS = Receivce Timeout Raw Status */
#define UART0_IMSC_TX	(1<<5)  /* TXRIS = Transmit Interrupt Raw Status */
#define UART0_IMSC_RX	(1<<4)	/* RXRIS = Receive Masked Interrupt Raw Status */
				/*    3 = DSRRIS (unsupported) */
				/*    2 = DCDRIS (unsupported) */
#define UART0_IMSC_CTS	(1<<1)	/* CTSRIS = nUARTCTS Raw Status */
				/*    0 = RIRIS (unsupported) */
#define UART0_RIS	(* (volatile unsigned int*)(UART0_BASE + 0x3C))
/* MIS = Masked Interrupt Status */
/*   10 = OEMIS = Overrun Interrupt Masked Status */
/*    9 = BEMIS = Break Interrupt Masked Status*/
/*    8 = PEMIS = Parity Interrupt Masked Status */
/*    7 = FEMIS = Framing Interrupt Masked Status */
/*    6 = RTMIS = Receivce Timeout Masked Status */
/*    5 = TXMIS = Transmit Interrupt Masked Status */
/*    4 = RXMIS = Receive Masked Interrupt Masked Status */
/*    3 = DSRMIS (unsupported) */
/*    2 = DCDMIS (unsupported) */
/*    1 = CTSMIS = nUARTCTS Masked Status */
/*    0 = RIMIS (unsupported) */
#define UART0_MIS	(* (volatile unsigned int*)(UART0_BASE + 0x40))
/* ICR = Interrupt Clear Register */
/*   10 = OEIC = Overrun Interrupt Clear */
/*    9 = BEIC = Break Interrupt Clear */
/*    8 = PEIC = Parity Interrupt Clear */
/*    7 = FEIC = Framing Interrupt Clear */
/*    6 = RTIC = Receivce Timeout Interrupt Clear */
/*    5 = TXIC = Transmit Interrupt Clear */
/*    4 = RXIC = Receive Masked Interrupt Status */
/*    3 = DSRMIC (unsupported) */
/*    2 = DCDMIC (unsupported) */
/*    1 = CTSMIC = nUARTCTS status */
/*    0 = RIMIC (unsupported) */
#define UART0_ICR	(* (volatile unsigned int*)(UART0_BASE + 0x44))
/* DMACR = DMA Control Register */
/*  This is disabled on the BCM2835 */
#define UART0_DMACR	(* (volatile unsigned int*)(UART0_BASE + 0x48))
/* ITCR = Test Control Register */
#define UART0_ITCR	(* (volatile unsigned int*)(UART0_BASE + 0x80))
/* ITIP = Test Control Register */
#define UART0_ITIP	(* (volatile unsigned int*)(UART0_BASE + 0x84))
/* ITOP = Test Control Register */
#define UART0_ITOP	(* (volatile unsigned int*)(UART0_BASE + 0x88))
/* TDR = Test Data Register */
#define UART0_TDR	(* (volatile unsigned int*)(UART0_BASE + 0x8C))
#define UART0_DR_PE (1<<9)

#define UART

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

// Define constants for the bit positions
#define VAR1_POS 0
#define VAR2_POS 3
#define VAR3_POS 6

// Define constants for the number of bits allocated to each variable
#define VAR1_BITS 3
#define VAR2_BITS 3
#define VAR3_BITS 18

// Define constants for the maximum values of each variable
#define MAX_VAR1 ((1 << VAR1_BITS) - 1)
#define MAX_VAR2 ((1 << VAR2_BITS) - 1)
#define MAX_VAR3 ((1 << VAR3_BITS) - 1)

#ifndef UART_H
#define UART_H

extern int uart_t_color;
extern int uart_b_color;
extern int baudrate;
extern int data_bits;
extern int stop_bits;
extern char parity;

#endif 

typedef unsigned int        uint32_t;

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

void uart_display_command_list();
void uart_display_command_detail(int command);

void uart_set_t_color();
void uart_set_b_color();

int uart_set_baudrate(char *s);
int uart_set_data_bits(char *s);
int uart_set_stop_bits(char *s);
int uart_set_parity(char *s);
int uart_set_handshaking(char *s);
int is_number(char *s);

void uart_print_board_revision(uint32_t revision);
void uart_mac_address_hex(unsigned int address1, unsigned int address2);


