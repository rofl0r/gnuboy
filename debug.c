#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>

#include "defs.h"
#include "cpu.h"
#include "mem.h"
#include "fastmem.h"
#include "regs.h"
#include "rc.h"
#include "cpuregs.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

static char *mnemonic_table[256] =
{
	"NOP",
	"LD BC,%w",
	"LD (BC),A",
	"INC BC",
	"INC B",
	"DEC B",
	"LD B,%b",
	"RLCA",
	"LD (%w),SP",
	"ADD HL,BC",
	"LD A,(BC)",
	"DEC BC",
	"INC C",
	"DEC C",
	"LD C,%b",
	"RRCA",
	"STOP",
	"LD DE,%w",
	"LD (DE),A",
	"INC DE",
	"INC D",
	"DEC D",
	"LD D,%b",
	"RLA",
	"JR %o",
	"ADD HL,DE",
	"LD A,(DE)",
	"DEC DE",
	"INC E",
	"DEC E",
	"LD E,%b",
	"RRA",
	"JR NZ,%o",
	"LD HL,%w",
	"LD (HLI),A",
	"INC HL",
	"INC H",
	"DEC H",
	"LD H,%b",
	"DAA",
	"JR Z,%o",
	"ADD HL,HL",
	"LD A,(HLI)",
	"DEC HL",
	"INC L",
	"DEC L",
	"LD L,%b",
	"CPL",
	"JR NC,%o",
	"LD SP,%w",
	"LD (HLD),A",
	"INC SP",
	"INC (HL)",
	"DEC (HL)",
	"LD (HL),%b",
	"SCF",
	"JR C,%o",
	"ADD HL,SP",
	"LD A,(HLD)",
	"DEC SP",
	"INC A",
	"DEC A",
	"LD A,%b",
	"CCF",
	"LD B,B",
	"LD B,C",
	"LD B,D",
	"LD B,E",
	"LD B,H",
	"LD B,L",
	"LD B,(HL)",
	"LD B,A",
	"LD C,B",
	"LD C,C",
	"LD C,D",
	"LD C,E",
	"LD C,H",
	"LD C,L",
	"LD C,(HL)",
	"LD C,A",
	"LD D,B",
	"LD D,C",
	"LD D,D",
	"LD D,E",
	"LD D,H",
	"LD D,L",
	"LD D,(HL)",
	"LD D,A",
	"LD E,B",
	"LD E,C",
	"LD E,D",
	"LD E,E",
	"LD E,H",
	"LD E,L",
	"LD E,(HL)",
	"LD E,A",
	"LD H,B",
	"LD H,C",
	"LD H,D",
	"LD H,E",
	"LD H,H",
	"LD H,L",
	"LD H,(HL)",
	"LD H,A",
	"LD L,B",
	"LD L,C",
	"LD L,D",
	"LD L,E",
	"LD L,H",
	"LD L,L",
	"LD L,(HL)",
	"LD L,A",
	"LD (HL),B",
	"LD (HL),C",
	"LD (HL),D",
	"LD (HL),E",
	"LD (HL),H",
	"LD (HL),L",
	"HALT",
	"LD (HL),A",
	"LD A,B",
	"LD A,C",
	"LD A,D",
	"LD A,E",
	"LD A,H",
	"LD A,L",
	"LD A,(HL)",
	"LD A,A",
	"ADD A,B",
	"ADD A,C",
	"ADD A,D",
	"ADD A,E",
	"ADD A,H",
	"ADD A,L",
	"ADD A,(HL)",
	"ADD A,A",
	"ADC A,B",
	"ADC A,C",
	"ADC A,D",
	"ADC A,E",
	"ADC A,H",
	"ADC A,L",
	"ADC A,(HL)",
	"ADC A",
	"SUB B",
	"SUB C",
	"SUB D",
	"SUB E",
	"SUB H",
	"SUB L",
	"SUB (HL)",
	"SUB A",
	"SBC A,B",
	"SBC A,C",
	"SBC A,D",
	"SBC A,E",
	"SBC A,H",
	"SBC A,L",
	"SBC A,(HL)",
	"SBC A,A",
	"AND B",
	"AND C",
	"AND D",
	"AND E",
	"AND H",
	"AND L",
	"AND (HL)",
	"AND A",
	"XOR B",
	"XOR C",
	"XOR D",
	"XOR E",
	"XOR H",
	"XOR L",
	"XOR (HL)",
	"XOR A",
	"OR B",
	"OR C",
	"OR D",
	"OR E",
	"OR H",
	"OR L",
	"OR (HL)",
	"OR A",
	"CP B",
	"CP C",
	"CP D",
	"CP E",
	"CP H",
	"CP L",
	"CP (HL)",
	"CP A",
	"RET NZ",
	"POP BC",
	"JP NZ,%w",
	"JP %w",
	"CALL NZ,%w",
	"PUSH BC",
	"ADD A,%b",
	"RST 0h",
	"RET Z",
	"RET",
	"JP Z,%w",
	NULL,
	"CALL Z,%w",
	"CALL %w",
	"ADC A,%b",
	"RST 8h",
	"RET NC",
	"POP DE",
	"JP NC,%w",
	NULL,
	"CALL NC,%w",
	"PUSH DE",
	"SUB %b",
	"RST 10h",
	"RET C",
	"RETI",
	"JP C,%w",
	NULL,
	"CALL C,%w",
	NULL,
	"SBC A,%b",
	"RST 18h",
	"LD (FF00+%b),A",
	"POP HL",
	"LD (FF00+C),A",
	NULL,
	NULL,
	"PUSH HL",
	"AND %b",
	"RST 20h",
	"ADD SP,%o",
	"JP HL",
	"LD (%w),A",
	NULL,
	NULL,
	NULL,
	"XOR %b",
	"RST 28h",
	"LD A,(FF00+%b)",
	"POP AF",
	"LD A,(FF00+C)",
	"DI",
	NULL,
	"PUSH AF",
	"OR %b",
	"RST 30h",
	"LD HL,SP%o",
	"LD SP,HL",
	"LD A,(%w)",
	"EI",
	NULL,
	NULL,
	"CP %b",
	"RST 38h"
};

static char *cb_mnemonic_table[256] =
{
	"RLC B",
	"RLC C",
	"RLC D",
	"RLC E",
	"RLC H",
	"RLC L",
	"RLC (HL)",
	"RLC A",
	"RRC B",
	"RRC C",
	"RRC D",
	"RRC E",
	"RRC H",
	"RRC L",
	"RRC (HL)",
	"RRC A",
	"RL B",
	"RL C",
	"RL D",
	"RL E",
	"RL H",
	"RL L",
	"RL (HL)",
	"RL A",
	"RR B",
	"RR C",
	"RR D",
	"RR E",
	"RR H",
	"RR L",
	"RR (HL)",
	"RR A",
	"SLA B",
	"SLA C",
	"SLA D",
	"SLA E",
	"SLA H",
	"SLA L",
	"SLA (HL)",
	"SLA A",
	"SRA B",
	"SRA C",
	"SRA D",
	"SRA E",
	"SRA H",
	"SRA L",
	"SRA (HL)",
	"SRA A",
	"SWAP B",
	"SWAP C",
	"SWAP D",
	"SWAP E",
	"SWAP H",
	"SWAP L",
	"SWAP (HL)",
	"SWAP A",
	"SRL B",
	"SRL C",
	"SRL D",
	"SRL E",
	"SRL H",
	"SRL L",
	"SRL (HL)",
	"SRL A",
	"BIT 0,B",
	"BIT 0,C",
	"BIT 0,D",
	"BIT 0,E",
	"BIT 0,H",
	"BIT 0,L",
	"BIT 0,(HL)",
	"BIT 0,A",
	"BIT 1,B",
	"BIT 1,C",
	"BIT 1,D",
	"BIT 1,E",
	"BIT 1,H",
	"BIT 1,L",
	"BIT 1,(HL)",
	"BIT 1,A",
	"BIT 2,B",
	"BIT 2,C",
	"BIT 2,D",
	"BIT 2,E",
	"BIT 2,H",
	"BIT 2,L",
	"BIT 2,(HL)",
	"BIT 2,A",
	"BIT 3,B",
	"BIT 3,C",
	"BIT 3,D",
	"BIT 3,E",
	"BIT 3,H",
	"BIT 3,L",
	"BIT 3,(HL)",
	"BIT 3,A",
	"BIT 4,B",
	"BIT 4,C",
	"BIT 4,D",
	"BIT 4,E",
	"BIT 4,H",
	"BIT 4,L",
	"BIT 4,(HL)",
	"BIT 4,A",
	"BIT 5,B",
	"BIT 5,C",
	"BIT 5,D",
	"BIT 5,E",
	"BIT 5,H",
	"BIT 5,L",
	"BIT 5,(HL)",
	"BIT 5,A",
	"BIT 6,B",
	"BIT 6,C",
	"BIT 6,D",
	"BIT 6,E",
	"BIT 6,H",
	"BIT 6,L",
	"BIT 6,(HL)",
	"BIT 6,A",
	"BIT 7,B",
	"BIT 7,C",
	"BIT 7,D",
	"BIT 7,E",
	"BIT 7,H",
	"BIT 7,L",
	"BIT 7,(HL)",
	"BIT 7,A",
	"RES 0,B",
	"RES 0,C",
	"RES 0,D",
	"RES 0,E",
	"RES 0,H",
	"RES 0,L",
	"RES 0,(HL)",
	"RES 0,A",
	"RES 1,B",
	"RES 1,C",
	"RES 1,D",
	"RES 1,E",
	"RES 1,H",
	"RES 1,L",
	"RES 1,(HL)",
	"RES 1,A",
	"RES 2,B",
	"RES 2,C",
	"RES 2,D",
	"RES 2,E",
	"RES 2,H",
	"RES 2,L",
	"RES 2,(HL)",
	"RES 2,A",
	"RES 3,B",
	"RES 3,C",
	"RES 3,D",
	"RES 3,E",
	"RES 3,H",
	"RES 3,L",
	"RES 3,(HL)",
	"RES 3,A",
	"RES 4,B",
	"RES 4,C",
	"RES 4,D",
	"RES 4,E",
	"RES 4,H",
	"RES 4,L",
	"RES 4,(HL)",
	"RES 4,A",
	"RES 5,B",
	"RES 5,C",
	"RES 5,D",
	"RES 5,E",
	"RES 5,H",
	"RES 5,L",
	"RES 5,(HL)",
	"RES 5,A",
	"RES 6,B",
	"RES 6,C",
	"RES 6,D",
	"RES 6,E",
	"RES 6,H",
	"RES 6,L",
	"RES 6,(HL)",
	"RES 6,A",
	"RES 7,B",
	"RES 7,C",
	"RES 7,D",
	"RES 7,E",
	"RES 7,H",
	"RES 7,L",
	"RES 7,(HL)",
	"RES 7,A",
	"SET 0,B",
	"SET 0,C",
	"SET 0,D",
	"SET 0,E",
	"SET 0,H",
	"SET 0,L",
	"SET 0,(HL)",
	"SET 0,A",
	"SET 1,B",
	"SET 1,C",
	"SET 1,D",
	"SET 1,E",
	"SET 1,H",
	"SET 1,L",
	"SET 1,(HL)",
	"SET 1,A",
	"SET 2,B",
	"SET 2,C",
	"SET 2,D",
	"SET 2,E",
	"SET 2,H",
	"SET 2,L",
	"SET 2,(HL)",
	"SET 2,A",
	"SET 3,B",
	"SET 3,C",
	"SET 3,D",
	"SET 3,E",
	"SET 3,H",
	"SET 3,L",
	"SET 3,(HL)",
	"SET 3,A",
	"SET 4,B",
	"SET 4,C",
	"SET 4,D",
	"SET 4,E",
	"SET 4,H",
	"SET 4,L",
	"SET 4,(HL)",
	"SET 4,A",
	"SET 5,B",
	"SET 5,C",
	"SET 5,D",
	"SET 5,E",
	"SET 5,H",
	"SET 5,L",
	"SET 5,(HL)",
	"SET 5,A",
	"SET 6,B",
	"SET 6,C",
	"SET 6,D",
	"SET 6,E",
	"SET 6,H",
	"SET 6,L",
	"SET 6,(HL)",
	"SET 6,A",
	"SET 7,B",
	"SET 7,C",
	"SET 7,D",
	"SET 7,E",
	"SET 7,H",
	"SET 7,L",
	"SET 7,(HL)",
	"SET 7,A"
};

static byte operand_count[256] =
{
	1, 3, 1, 1, 1, 1, 2, 1, 3, 1, 1, 1, 1, 1, 2, 1,
	1, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
	2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
	2, 3, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 3, 3, 3, 1, 2, 1, 1, 1, 3, 2, 3, 3, 2, 1,
	1, 1, 3, 1, 3, 1, 2, 1, 1, 1, 3, 1, 3, 1, 2, 1,
	2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 3, 1, 1, 1, 2, 1,
	2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 3, 1, 1, 1, 2, 1
};

#define MAX_BREAKPOINTS 5

int debug_trace = 0;
int *breakpoints = NULL;
volatile int step = 0;

extern void pcm_close();
extern void pcm_init();
extern byte *get_rom_content();


void stop_emulator(){
	pcm_close();
	step = 0;
}

void run_emulator(){
	pcm_init();
	step = 1;
}

void exit_emulator(){
	free(breakpoints);
	exit(0);
}

void busy_waiting(){
	while(step != 1);
}

void restart_emulator(){
	emu_reset();
	step = 1;
}

void clear_step(){
     step = 0;
}

int return_step(){
     return step;
}



void print_help(){

	printf(YEL "\n\nCOMMANDS AVAILABLE:\n\n" RESET);
	printf(
	 YEL "help:           print the available commands \n"
 		"cart:           information about the cartridge that is being used\n"
		"state:          print relevant information about the state of the CPU \n"
		"run:            run the emulator indefinitely\n"
	     "restart:        restart the emulator \n"
		"stop:           stop emulation \n"
		"break [addr]:   set a breakpoint at a certain address \n"
		"del [addr]:     delete a breakpoint at a certain address \n"
		"mdump:          obtain a memory dump in a file called 'memorydump' which is stored within gnuboy's installation directory \n"
		"                All the addresses containing ASCII values are not shown in the resulting file.\n"
          "                i.e: the information provided with the 'cart' command\n"
          "exit:           exit both the emulator and the debugger"
    	     "\n \n"
	 RESET
	);

	printf(WHT "\n\nOTHER OPTIONS:\n\n" RESET);
	printf(
	 WHT "screenshots:    press F7 key in order to get an instant snapshot of the emulator \n"
	     "screencasting:  press F8 to start screencasting and F9 to stop. The whole screen will be recorded, including sound"
	     "\n \n \n"
	 RESET
	);

}

void print_cart(){
     rom_info();
}

void exists_breakpoint(int breakpoint){

 for(int i = 0; i < MAX_BREAKPOINTS; i++)
	if(breakpoints[i] == breakpoint){
       breakpoints[i] = 0;
	  stop_emulator();
	  cpu_status(PC,1);
	  busy_waiting();
	}
}


int init_breakpoints(){

  breakpoints = malloc(MAX_BREAKPOINTS * sizeof(int));

  if(!breakpoints){
	printf("Could not initialize breakpoints. Stopping debugger \n");
	return errno;
  }
  memset(breakpoints,0,MAX_BREAKPOINTS);
  return 0;
}

void set_breakpoint(char *bkpoint){

  char *address = NULL;

  for(int i = 0; i < MAX_BREAKPOINTS; i++)
  if(breakpoints[i] == 0){

      strtok(bkpoint, " ");
	 address = strtok(NULL, " ");
	 breakpoints[i] = strtol(address, NULL, 16);
	 printf("Breakpoint at %x has been set\n",  strtol(address, NULL, 16));

	 return;
  }

  printf("Maximum number of breakpoints achieved. Delete a breakpoint before setting a new one\n");

}


void delete_breakpoint(char *bkpoint){

  int address = 0;
  char *addr = NULL;

  strtok(bkpoint, " ");
  addr = strtok(NULL, " ");
  address = strtol(addr, NULL, 16);

  for(int i = 0; i < MAX_BREAKPOINTS; i++)
	if(breakpoints[i] == address){
		breakpoints[i] == 0;
		printf("Deleted breakpoint at %x\n", address);
		return;
	}

  printf("Breakpoint not found\n");

}

void debugger_start(){


  char *line = NULL;
  ssize_t bufsize = 0;
  int size = 0;
  int end = 0;
  int bp = 0;

  bp = init_breakpoints();

  if(bp != 0){
	fprintf(stderr,"Error initializing breakpoints. Stopping debugger. %s \n", strerror(bp));
	return;
  }

  printf(BLU "\n\n\nWelcome to the gnuboy debugger version 1.0.3\n" RESET);
  printf(YEL "This version is being tested currently\n" RESET);
  printf(YEL "If you have any issues, contact me via:  cmn263@gmail.com\n" RESET );
  printf(WHT "\nType 'help' in order to display a list of the available commands\n\n" RESET);

  while(end == 0){

    printf(">");
    size = getline(&line, &bufsize, stdin);

	if (strcmp(line,"help\n") == 0){
	     print_help();
	}
	else if (strcmp(line,"cart\n") == 0){
		print_cart();
	}
     else if (strcmp(line,"state\n") == 0){
	     cpu_status(PC,1);
	}
	else if (strcmp(line,"mdump\n") == 0){
		memorydump();
	}
	else if (strcmp(line,"run\n") == 0){
		run_emulator();
	}
	else if (strcmp(line,"restart\n") == 0){
		restart_emulator();
	}
	else if (strcmp(line,"stop\n") == 0){
		stop_emulator();
	}
	else if (strstr(line,"break") != NULL){
	  set_breakpoint(line);
	}
	else if (strstr(line,"del") != NULL){
		delete_breakpoint(line);
	}
	else if (strcmp(line,"exit\n") == 0){
		exit_emulator();
	}
	else{
	if(strcmp(line,"\n") != 0){
		printf("Unrecognized command %s",line);
	}
  	}
  }

}

void debugger_init(){

	pthread_t th;

	int err = 0;
	err = pthread_create(&th, NULL, &debugger_start, NULL);
	if (err != 0)
	    printf("\nError when trying to run the debugger\n", strerror(err));

}


rcvar_t debug_exports[] =
{
	RCV_BOOL("trace", &debug_trace),
	RCV_END
};


void cpu_status(addr a, int c)
{
	static int i, j, k;
	static byte code;
	static byte ops[3];
	static int opaddr;
	static char mnemonic[256];
	static char *pattern;

	if (!debug_trace){
		printf("Debugging is not enabled. Aborting this function \n");
		return;
     }

	while (c > 0)
	{
		k = 0;
		opaddr = a;
		code = ops[k++] = readb(a); a++;
		if (code != 0xCB)
		{
			pattern = mnemonic_table[code];
			if (!pattern)
				pattern = "***INVALID***";
		}
		else
		{
			code = ops[k++] = readb(a); a++;
			pattern = cb_mnemonic_table[code];
		}
		i = j = 0;
		while (pattern[i])
		{
			if (pattern[i] == '%')
			{
				switch (pattern[++i])
				{
				case 'B':
				case 'b':
					ops[k] = readb(a); a++;
					j += sprintf(mnemonic + j,
						"%02Xh", ops[k++]);
					break;
				case 'W':
				case 'w':
					ops[k] = readb(a); a++;
					ops[k+1] = readb(a); a++;
					j += sprintf(mnemonic + j, "%04Xh",
						((ops[k+1] << 8) | ops[k]));
					k += 2;
					break;
				case 'O':
				case 'o':
					ops[k] = readb(a); a++;
					j += sprintf(mnemonic + j, "%+d",
						(n8)(ops[k++]));
					break;
				}
				i++;
			}
			else
			{
				mnemonic[j++] = pattern[i++];
			}
		}
		mnemonic[j] = 0;

		printf("PC = %06X\n", opaddr);

		switch (operand_count[ops[0]]) {
		case 1:
			printf("Source operand = %02X       \n", ops[0]);
			break;
		case 2:
			printf("Source operands = %02X %02X    \n", ops[0], ops[1]);
			break;
		case 3:
			printf("Source operands = %02X %02X %02X \n", ops[0], ops[1], ops[2]);
			break;
		}

		printf("Assembly instruction = %-16.16s\n", mnemonic);
		printf(
				"Registers: \n  SP=%04X \n  BC=%04X \n  DE=%04X"
				"\n  HL=%04X \n  A=%02X \n  F=%02X %c%c%c%c%c\n",
				SP,
				BC,
				DE,
				HL,
				A,
				F, (IME ? 'I' : '-'),
				((F & 0x80) ? 'Z' : '-'),
				((F & 0x40) ? 'N' : '-'),
				((F & 0x20) ? 'H' : '-'),
				((F & 0x10) ? 'C' : '-')
			);
			printf(
				"Interrupts: \n   IE=%02X IF=%02X LCDC=%02X STAT=%02X LY=%02X LYC=%02X\n",
				R_IE, R_IF, R_LCDC, R_STAT, R_LY, R_LYC
			);

		fflush(stdout);
		c--;
	}

	return;

}

void memorydump()
{

	int a = 0;
	static int i, j, k;
	static byte code;
	static byte ops[3];
	static int opaddr = 0;
	static char mnemonic[256];
	static char *pattern;
	byte *header = 0;

	FILE *fp;
	char aux[100];
	fp = fopen("memorydump", "w+");

	if(!fp){
		fprintf(stderr, "Could not initialize source file for memorydump: %s\n", strerror(errno));
		return;
	}

	if (!debug_trace){
		printf("Debugging is not enabled. Aborting memorydump \n");
		return;
     }

	while (a <= 0xFFFF)
	{
		k = 0;
		opaddr = a;

		print_regions(fp, a);

		code = ops[k++] = readb(a); a++;
		if (code != 0xCB)
		{
			pattern = mnemonic_table[code];
			if (!pattern){
				pattern = "***INVALID***";
			}
		}
		else
		{
			code = ops[k++] = readb(a); a++;
			pattern = cb_mnemonic_table[code];
		}

		i = j = 0;
		while (pattern[i])
		{
			if (pattern[i] == '%')
			{
				switch (pattern[++i])
				{
				case 'B':
				case 'b':
					ops[k] = readb(a); a++;
					j += sprintf(mnemonic + j,
						"%02Xh", ops[k++]);
					break;
				case 'W':
				case 'w':
					ops[k] = readb(a); a++;
					ops[k+1] = readb(a); a++;
					j += sprintf(mnemonic + j, "%04Xh",
						((ops[k+1] << 8) | ops[k]));
					k += 2;
					break;
				case 'O':
				case 'o':
					ops[k] = readb(a); a++;
					j += sprintf(mnemonic + j, "%+d",
						(n8)(ops[k++]));
					break;
				}
				i++;
			}
			else
			{
				mnemonic[j++] = pattern[i++];
			}
		}
		mnemonic[j] = 0;


          sprintf(aux, "0x%04X ", opaddr);
          fprintf(fp, aux);
		sprintf(aux, "%-16.16s\n", mnemonic);
		fprintf(fp, aux);

		fflush(stdout);
	}


	fclose(fp);

}

void print_regions(FILE *fp, int opaddr){


	switch(opaddr){
		case 0x0100:
				fprintf(fp, ";#---------------------------------- 16 KB ROM BANK #0 ----------------------------------#\n");
				fprintf(fp, ";#---------------------------------- BEGIN CODE EXECUTION POINT ----------------------------------#\n");
				break;
		case 0x0104:
				fprintf(fp, ";#---------------------------------- Scrolling Nintendo Graphic ----------------------------------#\n");
				break;
		case 0x0134:
				fprintf(fp, ";#---------------------------------- Title of the game in ASCII ----------------------------------#\n");
				break;
		case 0x4000:
				fprintf(fp, ";#---------------------------------- 16KB SWITCHABLE ROM BANK ----------------------------------#\n");
				break;
		case 0x8000:
				fprintf(fp, ";#---------------------------------- 8KB VIDEO RAM ----------------------------------#\n");
				break;
		case 0xA000:
				fprintf(fp, ";#---------------------------------- 8KB SWITCHABLE RAM BANK ----------------------------------#\n");
				break;
		case 0xC000:
				fprintf(fp, ";#---------------------------------- 8KB INTERNAL RAM ----------------------------------#\n");
				break;
		case 0xE000:
				fprintf(fp, ";#---------------------------------- ECHO OF 8KB INTERNAL RAM ----------------------------------#\n");
				break;
		case 0xFE00:
				fprintf(fp, ";#---------------------------------- SPRITE ATTRIB MEMORY (OAM) ----------------------------------#\n");
				break;
		case 0xFEA0:
				fprintf(fp, ";#---------------------------------- EMPTY BUT UNUSABLE FOR I/O ----------------------------------#\n");
				break;
		case 0xFF00:
				fprintf(fp, ";#---------------------------------- I/O PORTS ----------------------------------#\n");
				break;
		case 0xFF4C:
				fprintf(fp, ";#---------------------------------- EMPTY BUT UNUSABLE FOR I/O ----------------------------------#\n");
				break;
		case 0xFF80:
				fprintf(fp, ";#---------------------------------- INTERNAL RAM ----------------------------------#\n");
				break;
		case 0xFFFF:
				fprintf(fp, ";#---------------------------------- INTERRUPT ENABLE REGISTER ----------------------------------#\n");
					break;
	}

}
