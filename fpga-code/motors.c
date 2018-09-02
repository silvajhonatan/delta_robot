/* Code to control 3 stepper motors on an DE10-Lite FPGA */
#include "stdio.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "unistd.h"

#define MOTOR			SAIDA_C_BASE
#define SENSORES		ENTRADA_C_BASE

#define saida(porta,valor)  IOWR(porta, 0, valor)
#define entrada(porta)		IORD(porta, 0)
#define delay(MILISEGS)		usleep(1000UL * MILISEGS)

char comando_pc_serial, fins_de_curso, controle_dos_motores;
int n_passos = 0;
int inicio_curso, fim_curso;
int dir;

void enable_motors();
int set_motor_rotation(int dir,int state_control);
void control_motors(int dir,int state_control,int steps, int delayMicroseconds);

int main ()  {
    /*  
        Given motors configuration, it makes 5 revolutions
        and then waits 2 seconds and repeats in the other direction
    */ 
    /* 
        Nema 17, 200 steps per degree, in our driver 
        we've set 32 microsteps, so for 5 revolutions
        we have 200*32*5 steps
    */ 
    int dir = 1;
    int state_control = 0x92; // Used in the enable_motors();
    int steps = 200*32*5;  
    int delayMicroseconds = 350;
    enable_motors();
	while (1)  {
		control_motors(dir,state_control,steps,delayMicroseconds);
        delay(2000);
        dir = 0;
        control_motors(dir,state_control,steps,delayMicroseconds);
        delay(2000);
    }
}

void control_motors(int dir,int state_control,int steps, int delayMicroseconds){
    /* 
        First it updates the state_control of the 32bit IOWR pins 
        to the given direction 
        Then it changes the step (less significant bit) waits some time
        in microseconds and changes again
        first Step goes HIGH
        ---- ---- ---- ---- ---- ---0 1001 0010 XOR
        ---- ---- ---- ---- ---- ---0 0100 1001 
        next Step goes LOW
        ---- ---- ---- ---- ---- ---0 1101 1011 XOR
        ---- ---- ---- ---- ---- ---0 0100 1001

        ---- ---- ---- ---- ---- ---0 1001 0010
    */ 
    for(int how_many_steps = 0; how_many_steps < steps; how_many_steps++){
        state_control = set_motor_rotation(dir,state_control);
        state_control ^= 0x49;
        saida (MOTOR, state_control);
        usleep (200);
        state_control ^= 0x49;
        saida (MOTOR, state_control);
        usleep (200);
    }
}

int set_motor_rotation(int dir,int state_control){
    /* 
        If dir is equal to one, then it makes an
        "OR" bit by bit operation with 0x124 to 
        set all the motors direction to 1
        0000 0000 0000 0000 0000 0000 1001 0010
        0000 0000 0000 0000 0000 0001 0010 0100
        State_control variable with all dir and enable
        set in one
        0000 0000 0000 0000 0000 0001 1011 0110
        Else makes an "AND" operation with 0x0DB
        to invert the direction 
        0000 0000 0000 0000 0000 0001 1011 0110
        ---- ---- ---- ---- ---- ---0 1101 1011

    */
    if(dir) state_control |= 0x124;
    else state_control &= 0x0DB;
    return state_control;
}

void enable_motors(){
    /* 
        There's 32bits in our iowr port 
        The steppers have 3 commands, step,enable and dir,
        beeing dir the most significant bit
        So if I want to set enable 0 for the first motor I've
        to send the bit 2 to my variable "controle_dos_motores"
        2 => 0000 0000 0000 0000 0000 0000 0000 0010
        So if I want to enable all motors
        146 => 0000 0000 0000 0000 0000 0000 1001 0010
    */
    int enable_3_motors = 0x92;
	saida (MOTOR, enable_3_motors);
}