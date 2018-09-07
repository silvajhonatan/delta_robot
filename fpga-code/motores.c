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


void habilitar_motores(int variavel_de_controle);
int setar_direcao_do_motor(int direcao,int variavel_de_controle);
void controlar_motores(int direcao,int variavel_de_controle,int passos, int delayMicrosegundos);

int main ()  {
    /*  
        Primeiro criamos os parâmetros dos motores, depois
        o código faz com que o motor dê 5 revoluções espere
        dois segundos e dá 5 novas revoluções na direção oposta
    */ 
    /* 
        Nema 17, 200 passos por revolução, no nosso 
        driver nós setamos para 32 micropassos microsteps, então
        se nós quisermos que ele dê 5 voltas precisamos
        dar 200*32*5 passos
        Também setamos todos os bits da saída de 32 bits do FPGA
        em zero inicialmente.
    */ 
    int direcao = 1;
    int porta_saida_fpga_32bits = 0x00; 
    int passos = 200*32*5;  
    int delayMicrosegundos = 350;
    habilitar_motores(porta_saida_fpga_32bits);
	while (1)  {
		controlar_motores(direcao,
                          porta_saida_fpga_32bits,
                          passos,
                          delayMicrosegundos);
        delay(2000);
        dir = 0;
        controlar_motores(direcao,
                          porta_saida_fpga_32bits,
                          passos,
                          delayMicrosegundos);
        delay(2000);
    }
}

void controlar_motores(int direcao,
                       int variavel_de_controle,
                       int passos,
                       int delayMicrosegundos){
    /* 
        Primeiro é atualizada a porta de 32bits IOWR  
        para a dada direção. 
        Então ele muda o bit the step (bit menos significativo)
        espera um delay em microsegundos e muda o passo 
        novamente
        Exemplo prático
        Primeiro step vai para Nível lógico alto
        ---- ---- ---- ---- ---- ---0 1001 0010 XOR
        ---- ---- ---- ---- ---- ---0 0100 1001 
        próximo step leva para Nível lógico baixo
        ---- ---- ---- ---- ---- ---0 1101 1011 XOR
        ---- ---- ---- ---- ---- ---0 0100 1001

        ---- ---- ---- ---- ---- ---0 1001 0010
    */ 
    for(int quantos_passos = 0; quantos_passos < passos; quantos_passos++){
        variavel_de_controle = setar_direcao_do_motor(direcao,variavel_de_controle);
        variavel_de_controle ^= 0x49;
        saida (MOTOR, variavel_de_controle);
        usleep (delayMicrosegundos);
        variavel_de_controle ^= 0x49;
        saida (MOTOR, variavel_de_controle);
        usleep (delayMicrosegundos);
    }
}

int setar_direcao_do_motor(int direcao,int variavel_de_controle){
    /* 
        Se a direção for igual a 1,então faz uma
        operação "OU" bit a bit com o número hexa 
        0x124 para setar a direção de todos os motores 
        naquela direção.
        Exemplo
        0000 0000 0000 0000 0000 0000 1001 0010 
        OR
        0000 0000 0000 0000 0000 0001 0010 0100
        Agora a variável de controle fica com
        todos os pinos de dir setados em 1
        0000 0000 0000 0000 0000 0001 1011 0110
        Caso não seja 1 é feito uma operação
        "E" bit a bit com o hexa 0x0DB
        para inverter a direção 
        0000 0000 0000 0000 0000 0001 1011 0110 
        AND
        ---- ---- ---- ---- ---- ---0 1101 1011
        Final deixando apenas os pinos que já 
        estavam em nível lógico alto em 1
        ---- ---- ---- ---- ---- ---0 1001 0010
    */
    if(direcao) variavel_de_controle |= 0x124;
    else variavel_de_controle &= 0x0DB;
    return variavel_de_controle;
}

void habilitar_motores(int variavel_de_controle){
    /* 
        Existe 32 bits na porta de controle IOWR
        os motores de passo tem 3 comandos, step,
        enable e dir, sendo o bit mais significativo
        o dir, depois o enable e finalmente o step.
        Os drivers funcionam de maneira que quando
        é deixado o ENA em nível lógico baixo o motor
        está habilitado, e se você quiser desabilitar deixa
        em nível lógico alto
        Então se quisermos habilitar os 3 motores
        ao mesmo tempo devemos fazer uma operação
        OU com o hexa 0x92
        0x92 => 0000 0000 0000 0000 0000 0000 1001 0010
        ---- ---- ---- ---- ---- ---0 0000 0000
        OU
        ---- ---- ---- ---- ---- ---0 1001 0010
        ---- ---- ---- ---- ---- ---0 0000 0000
        Temos então no final a variável com todos os ENA em zero
        ---- ---- ---- ---- ---- ---0 1001 0010
    */
    variavel_de_controle |= 0x92;
	saida (MOTOR, variavel_de_controle);
}