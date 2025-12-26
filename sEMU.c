#include <stdint.h>
#include<stdio.h>
uint8_t PC = 0;
uint8_t R[4];
uint8_t M[16] = {0x8a,0x90,0xa0,0xb1,0x17,0x29,0xd1,0x42,0xe3};
uint8_t rd = 0;
uint8_t rs1 = 0;
uint8_t rs2 = 0;
uint8_t imm = 0;
uint8_t addr = 0;
uint8_t opcode = 0;
typedef enum{
   IFs,
    IDs,
    EXs
}state;
state cur = IFs;
void ID(uint8_t inst)
{
    opcode = (inst & 0xc0) >> 6;
    switch(opcode)
    {
        case 0:
            rd  = ((inst & 0x30) >> 4) & 0x03;
            rs1 = ((inst & 0x0c) >> 2) & 0x03;
            rs2 = (inst & 0x03);
        break;

        case 1:
             rs2 = (inst & 0x03);
        break;
        
        case 2:
            rd  = ((inst & 0x30) >> 4) & 0x03;
            imm = inst & 0x0f;
        break;

        case 3:
            addr = ((inst) >> 2) &0x0f;
            rs2 = (inst & 0x03);
        break;
    }
}
uint8_t IF()
{
    uint8_t inst = M[PC];
    return inst;
}
void EX()
{
    switch(opcode)
    {
        case 0:
            R[rd] = R[rs1] + R[rs2];
        break;
        case 1:
            printf("R[out] is %d\n",R[rs2]);
        break;        
        case 2:
            R[rd] = imm;
        break;

        case 3:
            if(R[0] != R[rs2]){
                PC = addr;
            }else
            {
                PC++;
            }
        break;
    }
}
void inst_cycle()
{
            int inst = IF();
            ID(inst);
            EX();
            if(opcode != 3)
            {
            PC++;
            }
  //  printf("PC is %d,R is %d %d %d %d\n",PC,R[0],R[1],R[2],R[3]);
}

int main(int argc,char *argv[])
{
    int temp = atoi(argv[1]);
    R[0] = temp;
    PC++;
    while(1){
        inst_cycle();
    }
}
