CSDS equ 30h       
CSDB equ 38h       
BUZZ equ P1.5      
SEG7 bit P1.6 	   
TEST bit P1.7	   
CZAS equ 79h	   
SS equ 78h
MM equ 77h
HH equ 76h
KBDS bit P3.5 	   

KBST equ 72h	   

org 0

	ljmp start     

org 0Bh			   
	setb F0 	   
	mov TH0 ,#226
;	cpl BUZZ
	reti

org 80h

start:
	mov SS,#30
	mov MM,#58
	mov HH,#12
	mov DPTR, #wzory              
	lcall przelicz

	;mov CZAS+6,#00111010B		  

	mov R7, #00000001b            
	mov R4, #4                  
				                 
	clr SEG7                     
	mov R1,#CZAS                 


	mov R2,#192 				 
	mov R3,#4

	mov IE,#10000010b			 
	mov TMOD,#01110000b;		 
	mov TH0 ,#226				 
	setb TR0					 

petla:
	jnb F0, petla				 
	clr F0						 
;	cpl BUZZ					 
	mov A,@R1                    
	inc R1                        
;	movc A,@A+DPTR                
	setb SEG7                    
	mov R0, #CSDB                
	movx @R0,A                   

	mov A, R7                    
	mov R0, #CSDS               
	movx @R0,A                   
	CLR SEG7                      
	mov C,KBDS
	jnb CY,noKey
	orl KBST,A 					 
	mov CZAS+6,KBST

noKey:


	rl A                        
	jnb ACC.7, noACC7             

	mov A,KBST
	jz bezObslugi
	cjne A,KBST+1, bezObslugi
    cjne A,KBST+2, bezObslugi
    cjne A,KBST+3, keyService
    sjmp bezObslugi
keyService:
	;4/16/32
	cjne A,#4 ,ACCnie4
	acall inkrementujSekundy
	sjmp bezObslugi

ACCnie4:
	cjne A,#16,ACCnie16
	acall inkrementujMinuty
	sjmp bezObslugi

ACCnie16:
    cjne A,#32,bezObslugi
	acall inkrementujGodziny

bezObslugi:

	mov A, #00000001b             
	mov R1, #CZAS+0               
	mov KBST+3,KBST+2
	mov KBST+2,KBST+1
    mov KBST+1,KBST+0
	mov KBST,#0
	mov CZAS+6,#0
noACC7:
	mov R7 , A                    


	djnz R2, jeszczeNieSekunda
	djnz R3, jeszczeNieSekunda
    cpl TEST   					  
    lcall inkrementujSekundy

	mov R2, #192
	MOV R3, #4

jeszczeNieSekunda:

;delay:
;	djnz R6, delay               
;	djnz R5, delay
;	djnz R4, delay
	mov R4, #4


	sjmp petla

inkrementujSekundy:
	inc SS
	mov R4,SS
	cjne R4,#60,jeszczeNie60
	mov SS,#0
inkrementujMinuty:
	inc MM
	mov R4,MM
	cjne R4,#60,jeszczeNie60
	mov MM,#0
inkrementujGodziny:
	inc HH
	mov R4,HH
	cjne R4,#24,jeszczeNie60
	mov HH,#0

jeszczeNie60:
	;lcall przelicz
	;ret

przelicz:

	mov A, SS
	mov B, #10
	div AB
	movc A,@A+DPTR
	mov CZAS+1,A
	mov CZAS+0,B
	mov A,B
	movc A,@A+DPTR
	mov CZAS+0,A

	mov A, MM
	mov B, #10
	div AB
	movc A,@A+DPTR
	mov CZAS+3,A
	mov A,B
	movc A,@A+DPTR
    mov CZAS+2,A

    mov A, HH
	mov B, #10
	div AB
	movc A,@A+DPTR
    mov CZAS+5,A
	mov A,B
	movc A,@A+DPTR
	mov CZAS+4,A

	ret


wzory:
db 00111111b,00000110b,01011011b,01001111b        ;WZORY NA WYSWIETLACZE 0-3
db 01100110b,01101101b,01111101b,00000111b        ;WZORY NA WYSWIETLACZE 4-8
db 01111111b,01101111b,01110111b,01111100b        ;WZORY NA WYSWIETLACZE 9-B

end