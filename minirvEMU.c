#include <stdint.h>
#include<stdlib.h>
#include<stdio.h>
#define GPRNUM 16
#define MEM_SIZE 1000
uint32_t PC = 0;
uint32_t R[GPRNUM];
uint32_t *M;
//sw_sb
uint8_t imm4_0 = 0;
uint8_t imm11_5 = 0;
//lui
int32_t imm20 = 0;
 
//add
uint8_t rs2 = 0;
uint8_t fun7 = 0;
//addi jarl
uint8_t rd = 0;
uint8_t fun3 = 0;
uint8_t rs1 = 0;
int32_t imm = 0;

uint32_t opcode =0;

void init_mem(char *filename)
{
    FILE *file = fopen(filename,"rb");
    
    fseek(file,0,SEEK_END);
    long file_size = ftell(file);
    fseek(file,0,SEEK_SET);

    if(file_size % sizeof(uint32_t) )
    {
        printf("err\n");
    }
    
    size_t inst_count = file_size / sizeof(uint32_t);
    
    M = (uint32_t*)malloc(file_size*sizeof(uint32_t));

    if(!M)fclose(file);

    size_t read_count = fread(M,sizeof(uint32_t),inst_count,file);
    
    printf("read_count %08lx",read_count);
    
    fclose(file);
    
}
void ID(uint32_t inst)
{
    opcode = (inst & 0x7f);
    switch(opcode)
    {
        case 0x13:
            rd = (inst >> 7) & 0x1f;
            fun3 = (inst >> 12) & 0x03;
            rs1 = (inst >> 15) & 0x1f;
            imm =(int32_t) inst >> 20 ;
        break;

        case 0x67:
            rd = (inst >> 7) & 0x1f;
            fun3 = (inst >> 12) & 0x03;
            rs1 = (inst >> 15) & 0x1f;
            imm = (inst >> 20) & 0xfff;
        break;

        case 0x33://add
            rd = (inst >> 7) & 0x1f;
            fun3 = (inst >> 12) & 0x03;
            rs1 = (inst >> 15) & 0x1f;
            rs2 = (inst >> 20) & 0x1f;
            fun7 = (inst >> 25) & 0x7f;
        break;
        case 0x37://lui
            rd = (inst >> 7) & 0x1f;
            imm20 =(int32_t) (inst >> 12);
        break;
        case 0x03://lw lbu
            rd = (inst >> 7) & 0x1f;
            fun3 = (inst >> 12) & 0x03;
            rs1 = (inst >> 15) & 0x1f;
            imm = (inst >> 20) & 0xfff;
        break;
        case 0x23:
            fun3 = (inst >> 12) & 0x03;
            rs1 = (inst >> 15) & 0x1f;
            rs2 = (inst >> 20) & 0x1f;
            imm4_0 = (inst >> 7) & 0x1f;
            imm11_5 = (inst >> 25) &0x7f;
        break; 
    
    }
    printf("%08x\n",inst);
}
uint32_t IF()
{
    uint32_t inst = M[PC/4];
    return inst;
}
void EX()
{
    switch(opcode)
    {
        case 0x13:
            printf("addi\n");
            if(rd != 0)
            {
                R[rd] = R[rs1] + imm; 
            }
            printf("r[rs1] %d+ imm%08xr[rd] = %08x\n",R[rs1],imm,R[rd]);
            printf("imm is %d",(int)imm);
        break;

        case 0x67:
            
            uint32_t temp = PC + 4;
            PC =(R[rs1] + imm);
            printf("imm %x r[rs1]%d\n",imm,R[rs1]);
            printf("jump to PC %d",PC);
            if(rd != 0)R[rd] = temp;
        break;

        case 0x33://add
            if(rd != 0)R[rd] = R[rs1] + R[rs2];
        break;

        case 0x37:
            if(rd != 0)R[rd] = imm20 << 12;
        break;
        case 0x03:
            if(fun3 == 2)//lw
            {
                if(rd != 0) R[rd] = M[R[rs1] + imm]; 
            }else if(fun3 == 4)
            {
                uint8_t sel = (R[rs1] + imm) & 0x03;
                if(sel == 0)
                {
                    if(rd != 0)R[rd] = M[R[rs1] + imm] & 0xff;
                }else if(sel == 1)
                {   
                    if(rd != 0)R[rd] = (M[R[rs1] + imm] & 0xff00) >> 8;
                }else if(sel == 2)
                {   
                    if(rd != 0)R[rd] = (M[R[rs1] + imm] & 0xff0000) >> 16;
                }else if(sel == 3)
                {
                    if(rd != 0)R[rd] = (M[R[rs1] + imm] & 0xff000000) >> 24;
                }
            }

        break;
        case 0x23:
            printf("sw\n");        
            int16_t imm_s = imm4_0 | (imm11_5 << 5);
            printf("imm_s %d\n",imm_s);
            uint32_t sumadd =(( (R[rs1] + imm_s))) ;

            printf("datat%08x %08x %08x\n",R[rs2],sumadd,R[rs1] + imm_s);
            if(fun3 == 2)
            {
               M[sumadd/4] = R[rs2]; 
            }else if(fun3 == 0)
            {
                uint8_t sel_sb=sumadd & 0x3;
                if(sel_sb == 0)
                {
                     M[sumadd/4] = R[rs2] & 0xff; 
                }else if(sel_sb == 1)
                {
                    M[sumadd/4] = (R[rs2] & 0xff) << 8; 
                }else if(sel_sb == 2)
                {
                    M[sumadd/4] = (R[rs2] & 0xff) << 16; 
                }else if(sel_sb == 3)
                {
                     M[sumadd/4] = (R[rs2] & 0xff) << 24; 
                }
            }
        break;
             
    
    }
}
void inst_cycle()
{
            uint32_t inst = IF();
            ID(inst);
            EX();
    for(int i=0;i< GPRNUM;i++)
    {
        printf("R[%d] is %08x ",i,R[i]);
        if(i % 4 ==0)
        {printf("\n");};
    } printf("\n");

}

int main(int argc,char *argv[])
{
    
    init_mem("sum.bin");
    int x=6000;
    while(x--){
    printf("PC is %x\n",PC);
        inst_cycle();
        if(opcode != 0x67)PC += 4;
    }
    free(M);
}
