#Compi 1.002

import os
import re
import sys

START_ADDRESS = 100
MEMORY_SIZE = 500
FILE_OUT = "code.code"
FILE_IN = "code.base"

NMS = ["CODE"         ,"PROG_POS"    ,"VAL_NULL","VAL_ONE","VAL_TWO","VAL_FOUR","VAL_SIX","VAL_EIGHT","VAL_SIXTEEN","VAL_NEG","MEM_END","ADD_A","ADD_B","ADD_OUT","SUB_A","SUB_B","SUB_OUT","MUL_A","MUL_B","MUL_OUT","DIV_A","DIV_B","DIV_OUT","REM_A","REM_B","REM_OUT","EQ_A","EQ_B","EQ_OUT","SHIFT_L_A","SHIFT_L_OUT","SHIFT_R_A","SHIFT_R_OUT","OR_A","OR_B","OR_OUT","AND_A","AND_B","AND_OUT","NEG_A","NEG_OUT","OUTPUT1","OUTPUT2","OUTPUT3","INPUT1","INPUT2","INPUT3","MONITOR","INTERUPT_JMP","PRE_INT_ADRESS","PROG_NEXT","PROG_NEXT_TWO","PROG_NEXT_THREE","PROG_NEXT_FOUR","SWITCH_A", "SWITCH_B", "SWITCH_S", "SWITCH_OUT","RAND","INTERUPT_MODE"]
ADR = [START_ADDRESS+1,START_ADDRESS ,0         ,1        ,2        ,3         ,4          ,5          ,6            ,7      ,9        ,10     ,11     ,12       ,13     ,14     ,15       ,16     ,17     ,18       ,19     ,20     ,21       ,22     ,23     ,24       ,25    ,26    ,27      ,28         ,29           ,30         ,31           ,32    ,34    ,35      ,36     ,37     ,38       ,39     ,40       ,42       ,43       ,44       ,45      ,46      ,47      ,48       ,49            ,50              ,51         ,52             ,53               ,54              ,55        ,56         ,57         ,58           ,59    ,60             ]

def replace(arr, old_val, new_val):
    for i in range(len(arr)):
        if arr[i] == old_val:
            arr[i] = new_val
    return arr


def programEND():
    if ERROR_COUNT != 0 :
        open(FILE_OUT, 'w').close() #vyprazdni tmp soubor
        print("\033[93m" + "Compilation cant be done!" + "\033[0m")
        sys.exit(1)
    else:
        print("\033[94m" + "Compilation DONE" + "\033[0m")
        sys.exit(0)
    

variableList = []
jmpList = []
TmpText = []


file = open(FILE_IN, 'r')

rawlines = file.readlines();
nCompiled = ""
ERROR = 0
ERROR_COUNT = 0
i = 0
for line in rawlines:  
    line = line.split('#', 1)[0]          #remove coments
    line = " ".join(line.split())         #remove coments
    if line.strip():
        jumpLabel = line.split("<-")
        if len(jumpLabel) >= 2:
            jmpList.append(jumpLabel[1]) # crete jump list
            i = i + 1
        nCompiled = nCompiled + line + '\n'

#check if jumps not same


variables = nCompiled.split('DATA:')[1].split('CODE:')[0].split()
instructions = nCompiled.split('CODE:')[1].replace('\t', ' ').splitlines() # crete instruction lines
if instructions[0] == '':
    instructions.pop(0)

file.close();


if (len(variables) % 2) != 0:
    print("\033[91m" + "ERROR[0]: Some variable dont have value" + "\033[0m")
    ERROR_TYPE = 2
    programEND()

for i in range(0,len(variables),2):
    variableList.append([variables[i],variables[i+1]]); # crete variable list
#check if not same variable names

JMPcount = nCompiled.split('CODE:')[1].count("<-")


StartAdress = int(START_ADDRESS + len(variables)/2 + 2 + JMPcount);
TmpText.append(str(StartAdress));
TmpText.append(str(StartAdress));
doneLineAdress = START_ADDRESS + 1;

for i in range(JMPcount):   #reserve space for jumps
    TmpText.append(str(StartAdress));
    doneLineAdress = doneLineAdress + 1;

for var in variableList:    #vrite variables
    TmpText.append(str(var[1])); 
    var[1] = doneLineAdress + 1;
    doneLineAdress = doneLineAdress + 1;


for line in instructions:
    jumpLabel = line.split("<-")
    onlyInst = line.split("<-")[0]
    if len(jumpLabel) >= 2: #line has jump identificator
        adr = 2 + jmpList.index(jumpLabel[1])
        TmpText[adr] = str(doneLineAdress+1); #write jump adress


    jumpLabel = line.split("{-")
    onlyInst = line.split("{-")[0]
    if len(jumpLabel) >= 2: #line has jump identificator
        variableList.append([jumpLabel[1],doneLineAdress+1]);
        #print(variableList);

    jumpLabel = line.split("}-")
    onlyInst = line.split("}-")[0]
    if len(jumpLabel) >= 2: #line has jump identificator
        variableList.append([jumpLabel[1],doneLineAdress+2]);
        #print(variableList);

    onlyInst = onlyInst.split();
    TmpText.append(onlyInst[0])
    TmpText.append(onlyInst[1])
    doneLineAdress = doneLineAdress + 2;



for i in range(len(NMS)):                # nahrazeni OPERA4N9CH PAM2T9 jmena na adresy
    TmpText = replace(TmpText,NMS[i],str(ADR[i]))

for i in range(len(variableList)):       #doplneni adress promenych
    TmpText = replace(TmpText,variableList[i][0],str(variableList[i][1]))

for i in range(len(jmpList)):           #doplneni adres jumpu
    TmpText = replace(TmpText,jmpList[i],str(START_ADDRESS+2+i))
   


open(FILE_OUT, 'w').close() #vyprazdni tmp soubor
output = open(FILE_OUT, "a")

for line in TmpText: # write to file
    if line.isnumeric():
        output.write(line + '\n')
    else:
        print("\033[91m" + "ERROR[" + str(ERROR_COUNT)+"]: ", line," is not defined!" + "\033[0m")
        ERROR_TYPE = 1
        ERROR_COUNT =+ 1;
        
output.close();

programEND()


