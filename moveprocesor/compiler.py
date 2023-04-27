import os
import re
import sys
from collections import Counter
from termcolor import colored


FILE_IN = "code.base"
FILE_OUT = "code.code"

START_ADDRESS = 100
MEMORY_SIZE = 500

NMS = ["CODE"         ,"PROG_POS"    ,"VAL_NULL","VAL_ONE","VAL_TWO","VAL_FOUR","VAL_SIX","VAL_EIGHT","VAL_SIXTEEN","VAL_NEG","MEM_END","ADD_A","ADD_B","ADD_OUT","SUB_A","SUB_B","SUB_OUT","MUL_A","MUL_B","MUL_OUT","DIV_A","DIV_B","DIV_OUT","REM_A","REM_B","REM_OUT","EQ_A","EQ_B","EQ_OUT","SHIFT_L_A","SHIFT_L_OUT","SHIFT_R_A","SHIFT_R_OUT","OR_A","OR_B","OR_OUT","AND_A","AND_B","AND_OUT","NEG_A","NEG_OUT","OUTPUT1","OUTPUT2","OUTPUT3","INPUT1","INPUT2","INPUT3","MONITOR","INTERUPT_JMP","PRE_INT_ADRESS","PROG_NEXT","PROG_NEXT_TWO","PROG_NEXT_THREE","PROG_NEXT_FOUR","SWITCH_A", "SWITCH_B", "SWITCH_S", "SWITCH_OUT","RAND","INTERUPT_MODE"]
ADR = [START_ADDRESS+1,START_ADDRESS ,0         ,1        ,2        ,3         ,4          ,5          ,6            ,7      ,9        ,10     ,11     ,12       ,13     ,14     ,15       ,16     ,17     ,18       ,19     ,20     ,21       ,22     ,23     ,24       ,25    ,26    ,27      ,28         ,29           ,30         ,31           ,32    ,34    ,35      ,36     ,37     ,38       ,39     ,40       ,42       ,43       ,44       ,45      ,46      ,47      ,48       ,49            ,50              ,51         ,52             ,53               ,54              ,55        ,56         ,57         ,58           ,59    ,60             ]


LOCAL_MACRO_IDNTFR = "_macro_local_jump_idx"

def replace(arr, old_val, new_val):
    for i in range(len(arr)):
        if arr[i] == old_val:
            arr[i] = new_val
    return arr

class jmpT():
    def __init__(self, name, index):
        self.name = name
        self.index = index

class lineO():
    def __init__(self, text,lineID,originFileNAME):
        self.text = text
        self.lineID = lineID
        self.originFileNAME = originFileNAME

class varO:
    def __init__(self, name, value , lineID,originFileNAME):
        self.name = name
        self.value = value
        self.lineID = lineID
        self.originFileNAME = originFileNAME

class macro():
    def __init__(self, name, content,macroStartID,originFileNAME):
        self.name = name
        self.content = content
        self.macroStartID = macroStartID
        self.originFileNAME = originFileNAME

class comandLine():
	def __init__(self, source, dest,linePOINT,sourcePOINT,destPOINT, lineID,originFileNAME):
		self.source = source
		self.dest = dest
		self.linePOINT = linePOINT #<-- crete variable and pas pointer to var
		self.sourcePOINT = sourcePOINT # {--- point to source adress
		self.destPOINT = destPOINT # }--- point to dest adress
		self.lineID = lineID
		self.originFileNAME = originFileNAME



tmpLines = []
linesArr = []

variables = []
inDataSection = 0

macroContent = []
macroName = ""
isInMacro = 0
macroStartId = 0
MACRO_LIST = []

CMD_LIST = []

VAR_IGNOR_CHAR = "-"


if "-h" in sys.argv or "-help" in sys.argv: 
	print("")
	print("comands:")
	print("	-memsze XXX    <- maximální délka programu (default je 500)")
	print("	-in XXX        <- compiled file (default je code.base)") 
	print("	-out XXX       <- output file (default je code.code)")
	print("	-shMerg        <- ukaže program spojený s knihoven a nahrazenými makry")
	print("	-shcode        <- ukaze hotovy kod v konzoli")
	print("")
	exit()

# get parameters
for i in range(len(sys.argv)):
	if sys.argv[i] == "-memsze":
		MEMORY_SIZE = int(sys.argv[i+1])
	if sys.argv[i] == "-in":
		FILE_IN = sys.argv[i+1]
	if sys.argv[i] == "-out":
		FILE_OUT = sys.argv[i+1]


# základní uprava
lineId = 1
for line in open(FILE_IN, 'r').readlines(): #načtení kodu z souboru 
	line = line.split('#', 1)[0]            #odstranení komentářu
	line = " ".join(line.split())           #odstranení nadbytečných tabů a mezer
	if line != '':
		linesArr.append(lineO(line,lineId,FILE_IN)) # každá linka má své číslo aby se dala dohledat chyba při překladu
	lineId = lineId + 1
if not linesArr[-1].text.startswith("FILE_END:"):
		linesArr.append(lineO("FILE_END:",lineId,FILE_IN)) # ukončení souboru pokud není ukončený aby byli ukončeny sekce data a code

# základní uprava END


#spojeni s ostatnimi soubory
INCLUDED_FILES_LINES = []   # include funguje jen v hlavním souboru
for line in linesArr:
	if line.text.startswith("INCLUDE"):
		sourceFile = line.text[len("INCLUDE"):]
		sourceFile = " ".join(sourceFile.split())

		lineId = 1
		for nFileLine in open(sourceFile, 'r').readlines(): #načtení kodu z souboru 
			nFileLine = nFileLine.split('#', 1)[0]            #odstranení komentářu
			nFileLine = " ".join(nFileLine.split())           #odstranení nadbytečných tabů a mezer
			if nFileLine != '':
				linesArr.append(lineO(nFileLine,lineId,sourceFile)) # každá linka má své číslo aby se dala dohledat chyba při překladu
			lineId = lineId + 1
		if not linesArr[-1].text.startswith("FILE_END:"):
			linesArr.append(lineO("FILE_END:",lineId,sourceFile)) # ukončení souboru pokud není ukončený aby byli ukončeny sekce data a code



#extrakce promenych
inDataSection = 0
for line in linesArr:
	if line.text.startswith("DATA:"):
		inDataSection = 1
	elif line.text.startswith("CODE:"):
		inDataSection = 0
	elif line.text.startswith("FILE_END:"):
		inDataSection = 0
	elif inDataSection == 1:
		varDuo = line.text.split()
		variables.append(varO(varDuo[0],varDuo[1],line.lineID,line.originFileNAME))

#kontrola střetu s registry
err = 0
for var in variables:
	for rgNME in NMS:
		if rgNME == var.name:
			print(colored('Promena ','red') + colored(var.name,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(var.originFileNAME +":"+ str(var.lineID),'dark_grey') + colored(' je stejné jako jmeno registru!', 'red'))
			err = 1

# konrola duplikatu
for var in variables:
	for varTWO in variables:
		if var.name == varTWO.name and var.lineID < varTWO.lineID and var.name != VAR_IGNOR_CHAR:
			print(colored('Promena ','red') + colored(var.name,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(var.originFileNAME +":"+ str(var.lineID),'dark_grey') + colored(' je definavana znovu na lince ', 'red') + colored(varTWO.originFileNAME +":"+ str(varTWO.lineID),'dark_grey') + colored(' !','red'))
			err = 1
#extrakce promenych END

if err == 1:
	print('\n\33[33m' + 'Překlad zastaven kvuli nesprávně definovým makrum :) !'+'\33[0m')
	exit(1)

# uložení maker 
for line in linesArr:  
	if line.text.startswith("MACRO "):
		macroName = line.text[len("MACRO "):]  # uložení všeho za "MACRO"
		macroName = " ".join(macroName.split())  # očištění mazvu makra o nadbytečné mezery
		if macroName.endswith(":"):	# očištění nazvu makra o ':' na konci makro
			macroName = macroName[:-1]
		macroStartId = line.lineID
		macroStartFileId = line.originFileNAME
		isInMacro = 1					  # je makrem
		#kontrola jestli makro není nejkaý předdefnovaný register
		
	elif line.text.startswith("MACRO_END"): 
		isInMacro = 0
		MACRO_LIST.append(macro(macroName,macroContent,macroStartId,macroStartFileId))	# prodani ulozeneho makra
		macroContent = []
		macroName = ""
	else:
		if isInMacro == 1:
			macroContent.append(lineO(line.text,line.lineID,line.originFileNAME))
		else:
			tmpLines.append(lineO(line.text,line.lineID,line.originFileNAME));

# detekce chyby
for macro in MACRO_LIST:
	for rgNME in NMS:
		if rgNME == macro.name:
			print(colored('Macro ','red') + colored(macro.name,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(macro.originFileNAME +":"+ str(macro.macroStartID),'dark_grey') + colored(' je stejné jako jmeno registru!', 'red'))
			err = 1

# konrola duplikatu
for macro in MACRO_LIST:
	for macroTWO in MACRO_LIST:
		if macro.name == macroTWO.name and macro.macroStartID < macroTWO.macroStartID:
			print(colored('Macro ','red') + colored(macro.name,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(macro.originFileNAME +":"+ str(macro.macroStartID),'dark_grey') + colored(' je definavana znovu na lince ', 'red') + colored(macroTWO.originFileNAME + ":" + str(macroTWO.macroStartID),'dark_grey') + colored(' !','red'))
			err = 1

linesArr = tmpLines
tmpLines = []
# uložení maker END

if err == 1:
	print()
	print(colored('Překlad zastaven kvuli chybám extrakce maker a proměných :) !','yellow'))
	exit(1)

def replaceMacro(linesArr):
	err = 0
	replacedLines = []
	macroNUM = 0
	for line in linesArr:
		isMacroLine = 0
		for macroDTA in MACRO_LIST:
			if line.text.startswith(macroDTA.name+" "): # když na řádku je makro
				macroNUM = macroNUM + 1
				macroArg = line.text[len(macroDTA.name):].split() # vzaní arumentů
				isMacroLine = 1
				lnArg = ""
				for lpoin in macroArg: # uloží ukazovátka řádku volání makra
					if lpoin.startswith('<-') or lpoin.startswith('{-') or lpoin.startswith('}-'):
						macroArg.remove(lpoin)
						lnArg = lnArg + lpoin
						if lnArg[0] != " ":
							lnArg = " " + lnArg

				pointersList = []
				for macroLine in macroDTA.content: #uložení ukazovátek aby se mohli nahradit
					for lpoin in macroLine.text.split():
						if lpoin.startswith('<-') or lpoin.startswith('{-') or lpoin.startswith('}-'):
							pointersList.append([lpoin,lpoin+LOCAL_MACRO_IDNTFR + str(macroNUM)])
				#print(pointersList)
				for macroLine in macroDTA.content:
					
					#nahrazení ukazovátek maker 
					#
					localLine = ""

					for word in macroLine.text.split():
						nwword = word
						for name,pseudo in pointersList:
							if word == name:
								nwword = pseudo
							elif word == name[2:]:
								nwword = pseudo[2:]
						localLine = localLine + " " + nwword
					print(localLine)
					for i in range(len(macroArg)):
						# nahrazeni ukazovatek jejich lokalnim pseudomymem
						macroLine = lineO(localLine.replace(f"${i}", macroArg[i]) + lnArg,macroLine.lineID,macroLine.originFileNAME) # nahrazeni parametru makra
						lnArg = "" #připojí jen na první řádek toto zamezí připojení na ostatní radky
					if macroLine.text.find("$") != -1: #makro obsahuje argument který není definován
						print(colored('Makro "','red') + colored(line.text.split()[0],'dark_grey') + colored('" použité na lince ','red') + colored(line.originFileNAME +":"+ str(line.lineID),'dark_grey') + colored(' vyžaduje více parametrů!','red') + colored('" Makro je definovano na lince ','red') +  colored(macroLine.originFileNAME +":"+ str(macroLine.lineID),'dark_grey') + colored('!','red') )
						err = 1 
					replacedLines.append(macroLine)
		if isMacroLine == 0:
			replacedLines.append(line)
	if err == 1:
		print('\n' + colored('Překlad zastaven kvuli chybnému zapsání makra!','yellow'))
		exit(1)
		
	return replacedLines


#simplifikace maker
for macroInd in range(len(MACRO_LIST)): #ASI FUNGUJE IDK
	MACRO_LIST[macroInd].content = replaceMacro(MACRO_LIST[macroInd].content);#ASI FUNGUJE IDK

for macroInd in range(len(MACRO_LIST)):
	print("mnm:",MACRO_LIST[macroInd].name)
	for line in MACRO_LIST[macroInd].content :
		print("  mcont:",line.text) # check for macro line 
				#nahrazeni macra







# nahrazení maker a uložení do souboru

linesArr = replaceMacro(linesArr) # dodělat lokální skoky makra

# nahrazení maker a uložení do souboru END



# extrakce prikazu
inDataSection = 0
for line in linesArr:
	if line.text.startswith("CODE:"):
		inDataSection = 1
	elif line.text.startswith("DATA:"):
		inDataSection = 0
	elif line.text.startswith("FILE_END:"):
		inDataSection = 0
	elif inDataSection == 1:
		cmdOnLine = line.text.split()
		cmd_arr = []
		sourcePOINT = ""
		destPOINT = ""
		linePOINT = ""
		for cmdItem in cmdOnLine:
			if cmdItem.startswith("<-"):
				linePOINT = cmdItem[len("<-"):]
			elif cmdItem.startswith("}-"):
				destPOINT = cmdItem[len("}-"):]
			elif cmdItem.startswith("{-"):
				sourcePOINT = cmdItem[len("{-"):]
			else:
				cmd_arr.append(cmdItem)
		if len(cmd_arr) == 2:
			CMD_LIST.append(comandLine(cmd_arr[0], cmd_arr[1],linePOINT,sourcePOINT,destPOINT, line.lineID,line.originFileNAME))
		elif len(cmd_arr) > 2:
			print(colored('Linka "','red') + colored(line.text,'light_grey',attrs=["underline"]) + colored('" na řádku ','red') + colored(macro.originFileNAME +":"+ str(macro.macroStartID),'dark_grey') + colored(' obsahuje více než 2 registry!','red'))
			err = 1
		else:
			print(colored('Linka "','red') + colored(line.text,'light_grey',attrs=["underline"]) + colored('" na řádku ','red') + colored(macro.originFileNAME +":"+ str(macro.macroStartID),'dark_grey') + colored(' neobsahuje 2 registry (scr,dest)!','red'))
			err = 1
# extrakce prikazu END

# zjisteni kolize ukazovatek 
for cmd in CMD_LIST:
	for rgNME in NMS:
		if rgNME == cmd.sourcePOINT:
			print(colored('Ukazovatako ','red') + colored("{-" + cmd.sourcePOINT,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(cmd.originFileNAME +":"+ str(cmd.lineID),'dark_grey') + colored(' je stejné jako jmeno registru!', 'red'))
			err = 1
		if rgNME == cmd.destPOINT:
			print(colored('Ukazovatako ','red') + colored("}-" + cmd.destPOINT,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(cmd.originFileNAME +":"+ str(cmd.lineID),'dark_grey') + colored(' je stejné jako jmeno registru!', 'red'))
			err = 1
		if rgNME == cmd.linePOINT:
			print(colored('Ukazovatako ','red') + colored("<-" + cmd.destPOINT,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(cmd.originFileNAME +":"+ str(cmd.lineID),'dark_grey') + colored(' je stejné jako jmeno promene!', 'red'))
			err = 1
	for varNME in variables:
		if varNME.name == cmd.sourcePOINT:
			print(colored('Ukazovatako ','red') + colored("{-" + cmd.sourcePOINT,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(cmd.originFileNAME +":"+ str(cmd.lineID),'dark_grey') + colored(' je stejné jako jmeno promene!', 'red'))
			err = 1
		if varNME.name == cmd.destPOINT:
			print(colored('Ukazovatako ','red') + colored("}-" + cmd.destPOINT,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(cmd.originFileNAME +":"+ str(cmd.lineID),'dark_grey') + colored(' je stejné jako jmeno promene!', 'red'))
			err = 1
		if varNME.name == cmd.linePOINT:
			print(colored('Ukazovatako ','red') + colored("<-" + cmd.sourcePOINT,'light_grey',attrs=["underline"]) + colored(' definovane na lince ','red') + colored(cmd.originFileNAME +":"+ str(cmd.lineID),'dark_grey') + colored(' je stejné jako jmeno promene!', 'red'))
			err = 1




duplicits_tst = []

for cmd in CMD_LIST:
	#print(cmd.sourcePOINT,cmd.destPOINT,cmd.linePOINT)
	if cmd.sourcePOINT:
		duplicits_tst.append(cmd.sourcePOINT)
	if cmd.destPOINT:
		duplicits_tst.append(cmd.destPOINT)
	if cmd.linePOINT:
		duplicits_tst.append(cmd.linePOINT)

duplicates = set([x for x in duplicits_tst if duplicits_tst.count(x) > 1])

if len(duplicates) != 0:
	print(colored('Nalezena duplicitní ukazovátka! ','red') + colored(duplicates,'dark_grey'))
	err = 1



if err == 1:
	print()
	print(colored('Překlad zastaven kvuli chybám extrakce maker a proměných :) !','yellow'))
	exit(1)



# zde máme všechny proměné
# a taky k´d rozdělený 
# makra jsou aplikovány


if "-shMerg" in sys.argv:
	print(colored("SHOWING PROCESED CODE!!",'blue'))
	print("DATA:")
	for var in variables:
		print(var.name,var.value)
	print("CODE:")
	for cmdL in CMD_LIST:
		linePOINT = ""
		sourcePOINT = ""
		destPOINT = ""
		if cmdL.sourcePOINT:
			sourcePOINT = "{-" + cmdL.sourcePOINT
		if cmdL.destPOINT:
			destPOINT = "}-" + cmdL.destPOINT
		if cmdL.linePOINT:
			linePOINT = "<-" + cmdL.linePOINT
		print(cmdL.source,cmdL.dest,linePOINT,sourcePOINT,destPOINT);
	print(colored("CODE END!!",'blue'))

COMPILED_CODE = []	
COMPILED_CODE_ER_LINER = []
COMPILED_CODE_ER_FILE = []

JMP_ARR = []
SORCEDR_ARR = []
DESTDR_ARR = []
tVAR_ARR = []

JMP_ARR.append(jmpT("CODE",0)) # podržení mista na skoky
JMP_ARR.append(jmpT("CODE",1))
COMPILED_CODE.append(0)
COMPILED_CODE.append(0)

COMPILED_CODE_ER_LINER.append(0)
COMPILED_CODE_ER_LINER.append(0)

COMPILED_CODE_ER_FILE.append(0)
COMPILED_CODE_ER_FILE.append(0)

for cmdL in CMD_LIST:
	if cmdL.linePOINT:
		JMP_ARR.append(jmpT(cmdL.linePOINT,len(COMPILED_CODE))) # podržení místa na skokové proměne
		COMPILED_CODE.append(0)
		COMPILED_CODE_ER_LINER.append(cmdL.lineID)
		COMPILED_CODE_ER_FILE.append(cmdL.originFileNAME)

for var in variables:
	tVAR_ARR.append(jmpT(var.name,len(COMPILED_CODE)))
	COMPILED_CODE.append(int(var.value))
	COMPILED_CODE_ER_LINER.append(var.lineID)

COMPILED_CODE[0] = len(COMPILED_CODE) + START_ADDRESS
COMPILED_CODE[1] = len(COMPILED_CODE) + START_ADDRESS


for inst in CMD_LIST: # roztaženi programu na radky
	for cmdL in JMP_ARR:
		if inst.destPOINT:
			SORCEDR_ARR.append(jmpT(inst.destPOINT,len(COMPILED_CODE)+1))
		if inst.sourcePOINT:
			DESTDR_ARR.append(jmpT(inst.sourcePOINT,len(COMPILED_CODE)))

		if cmdL.name == inst.linePOINT:
			COMPILED_CODE[cmdL.index] = len(COMPILED_CODE) + START_ADDRESS # nahrazení skokové adresi
	COMPILED_CODE.append(inst.source)
	COMPILED_CODE.append(inst.dest)
	COMPILED_CODE_ER_LINER.append(inst.lineID)
	COMPILED_CODE_ER_LINER.append(inst.lineID)
	COMPILED_CODE_ER_FILE.append(inst.originFileNAME)
	COMPILED_CODE_ER_FILE.append(inst.originFileNAME)






#nahrazeni instrukci

for i in range(len(NMS)):                # nahrazenich adres registru
    COMPILED_CODE = replace(COMPILED_CODE,NMS[i],int(ADR[i]))

for var in tVAR_ARR:					# nahrazeni promenich
    for i in range(len(COMPILED_CODE)):
        if COMPILED_CODE[i] == var.name:
            COMPILED_CODE[i] = var.index + START_ADDRESS

for var in JMP_ARR:					#nahrazeni skoku
    for i in range(len(COMPILED_CODE)):
        if COMPILED_CODE[i] == var.name:
            COMPILED_CODE[i] = var.index + START_ADDRESS

for scra in SORCEDR_ARR:					# nahrazeni adres prikazu
    for i in range(len(COMPILED_CODE)):
        if COMPILED_CODE[i] == scra.name:
            COMPILED_CODE[i] = scra.index + START_ADDRESS

for desta in DESTDR_ARR:					# nahrazeni adres prikazu
    for i in range(len(COMPILED_CODE)):
        if COMPILED_CODE[i] == desta.name:
            COMPILED_CODE[i] = desta.index + START_ADDRESS

#pridat nahrazeni adress





# kontrola nahrazeni všech radku
err = 0
for i in range(len(COMPILED_CODE)):
	if type(COMPILED_CODE[i]) != int:
		err = 1
		print(colored("Nerozpozaný výraz: ",'red') + colored(COMPILED_CODE[i],'dark_grey') +  colored(" na radku: ",'red') + colored(COMPILED_CODE_ER_FILE[i] +":"+ str(COMPILED_CODE_ER_LINER[i]),'dark_grey') +  colored("!",'red'))
		exit(1)

if err == 1 :
	print()
	print(colored('Překlad zastaven kvuli chybám v numerizaci :) !','yellow'))
	exit(1)


# kontrola velikost
if(MEMORY_SIZE < len(COMPILED_CODE)):
	print()
	print(colored("Výsledný kód přesahl velikost paměti! ",'yellow') + colored("use -memsze XXX to change targeted memory size",'dark_grey') )
	exit(1)



if "-shcode" in sys.argv:
	print(colored("CODE START:",'blue'))
	for cd in COMPILED_CODE:
		print(cd)
	print(colored("CODE END!",'blue'))

open(FILE_OUT, 'w').close() #vyprazdni tmp soubor
output = open(FILE_OUT, "a")

for line in COMPILED_CODE: # write to file
    output.write(str(line) + '\n')

        
output.close();


print("done")
exit(0)