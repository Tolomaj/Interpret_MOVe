import os 
import re

START_ADDRESS = 100
MAX_LINES = 200
ADD_PROGRAM_OFFSET = 1
ADD_PROGRAM_START_DUP = 1


NMS = ["PROG_START"   ,"PROG_POS"    ,"VAL_NULL","VAL_ONE","VAL_TWO","VAL_FOUR","VAL_EIGHT","VAL_SIXTEEN","VAL_NEG","ADD_A","ADD_B","ADD_OUT","SUB_A","SUB_B","SUB_OUT","MUL_A","MUL_B","MUL_OUT","DIV_A","DIV_B","DIV_OUT","REM_A","REM_B","REM_OUT","EQ_A","EQ_B","EQ_OUT","SHIFT_L_A","SHIFT_L_OUT","SHIFT_R_A","SHIFT_R_OUT","OUTPUT1","OUTPUT2","OUTPUT3","INPUT1","INPUT2","INPUT3","MONITOR","EXIT","RUN"]
ADR = [START_ADDRESS+1,START_ADDRESS ,0         ,1        ,2        ,3         ,4          ,5            ,6        ,10     ,11     ,12       ,13     ,14     ,15       ,16     ,17     ,18       ,19     ,20     ,21       ,22     ,23     ,24       ,25    ,26    ,27      ,28         ,29           ,30         ,31           ,42       ,43       ,44       ,45      ,56      ,47      ,48       ,49    ,50   ]

if ADD_PROGRAM_START_DUP:
    START_ADDRESS = START_ADDRESS + 1


open('tmpCODE.tmp', 'w').close() #vyprazdni tmp soubor

file = open('code.txt', 'r')
TMPCODE = open("tmpCODE.tmp", "a")

Lines = file.readlines()
 
count = 0
firstOP = 0
varCount = 0
# Strips the newline character
for line in Lines:
    procLine = line.split('#', 1)[0]
    procLine = " ".join(procLine.split())

    if len(procLine.split()) >= 2 and ADD_PROGRAM_OFFSET == 1:
        procLine = re.sub(r'\b\d+\b', lambda m: str(int(m.group())+START_ADDRESS), procLine)

    if procLine.strip():
        TMPCODE.write(procLine)
        TMPCODE.write('\n')

    if firstOP == 0 and len(procLine.split()) >= 2:
        varCount = count
        firstOP = 1
    count = count + 1 
TMPCODE.close()



file1 = open('tmpCODE.tmp', 'r')
fileTXT = file1.read()

open('code.code', 'w').close()
output = open("code.code", "a")


count = 0
for name in NMS:
    fileTXT = fileTXT.replace(name,str(ADR[count]));
    count += 1

fileTXT = fileTXT.replace(' ', '\n')
output.write(str(START_ADDRESS+varCount+1))
output.write('\n')
if ADD_PROGRAM_START_DUP == 1 :             # add start adress on file start !! this shifts all variables by one
    output.write(str(START_ADDRESS+varCount+1))
output.write('\n')
output.write(fileTXT)

