# Toto je interpret a debugovací program pro MOVe 
![Snímek obrazovky 2023-04-08 142912](https://user-images.githubusercontent.com/59420562/230721129-a7f1dd1f-1905-463d-9922-a4c3632c4acd.png)
# Co interpret umožňuje?
Umožňuje náhled na která instrukce se v kódu provádí.
Má příkazi na debug:
```python
run 		#spustí simulaci
stop 		#zastaví simulaci
key 		#připojí klávesnici na vstup
	
hvar 		#zvírazní co adresi proměných
hvare 		#zvírazní konec proměnného prostoru 
hjmp 		#zvýrazní zápis do pozicového registru
hall 		#zapne všechno zvýraznování
clsr 		#překreslí UI konzole.                   // občas při zvětšování dělá konzole zmatky

step      	#provede jednu instrukci
clock XXX 	#nastaví rychlost hodin použitých pokud program běží
peek XXX  	#zobrazí co register obsahuje
mv AAA BBB 	#přesune obsah z registru AAA do registru BBB // v debug verzi je třeba zadávat jako mv001025 // 1 >> 25

load     	#compile and load program
open     	#open program file (in separate window probably notepad)
```


# Co je MOVe?
MOVe je programovací jazyk obsahující jen jedinou instrukci a to přesuň z adresy na adresu.

# Jak se v MOVe například sčítá když umí jen přesouvat registry?
Procesor má mnoho registrů které zvládají jednoduché operace.
Například:
```
MOJE_CISLO ADD_A
TVOJE_CISLO ADD_B
ADD_OUT MONITOR
```
Tento kus kódu provede sečtení čísla v registru MOJE_CISLO a čísla TVOJE_CISLO,
které zapíše na register MONITOR (viditelný v debugeru).

## komentáře v jazyce MOVe:
Vše o je za # je považované za komentář

# Registry jazyka MOVe:

Regisry konstant(obsahují vždy číslo):
```python
VAL_NUL     #obsahuje 0
VAL_ONE     #obsahuje 1
VAL_TWO     #obsahuje 2
VAL_FOUR    #obsahuje 4
VAL_SIX     #obsahuje 6
VAL_EIGHT   #obsahuje 8
VAL_SIXTEEN #obsahuje 16 
VAL_NEG     #obsahuje -1

RAND        #obsahuje náhodné číslo
```
Speciální Registry: 
```python
PROG_START
PROG_POS         #obsahuje adresu dalšího příkazu
PROG_NEXT	 #obsahuje adresu příkazu po 2 provedení
PROG_NEXT_TWO    #obsahuje adresu příkazu po 3 provedení
PROG_NEXT_THREE  #obsahuje adresu příkazu po 4 provedení
PROG_NEXT_FOUR   #obsahuje adresu příkazu po 5 provedení

G_POINTER        #register určený pro ukládání ukazování do paměi // !nedodělané
MAX_MEM          #obsahuje velikost maximální paměti
INTERUPT_JMP     #obsahuje adresu skoku při interuptu
INTERUPT_MODE    #obsahuje zapnutí interuptu. Reaguje na [DISPLAY1], 0 = vyplé, 1 = při změně na HIGH, 2 = při změně na LOW, 3 = při HIGH i LOW. 
PRE_INT_ADRESS   #obsahuje adresu ze kterého se do přerušení skočilo

MONITOR	  	 #register napojený na monitor v simulátoru
	
OUTPUT1
OUTPUT2
OUTPUT3

INPUT1
INPUT2
INPUT3
```
Matematické Registry(slouží k matematice):
```python
ADD_OUT = ADD_A + ADD_B
SUB_OUT = SUB_A - SUB_B
MUL_OUT = MUL_A * MUL_B
DIV_OUT = DIV_A / DIV_B
REM_OUT = REM_A % REM_B
EQ_OUT  = EQ_A == EQ_B
OR_OUT  = OR_A  | OR_B
AND_OUT = AND_A && AND_B

SHIFT_L_OUT = SHIFT_L_A << 1
SHIFT_R_OUT = SHIFT_R_A >> 1

SWITCH_OUT = SWITCH_S ? SWITCH_A : SWITCH_B
		
NEG_OUT = ~NEG_A 
```
# Ukazování na adresy v jazyka MOVe:

## Pozice řádku
Toto vytvoří proměnou [NAME] obsahující číslo řádku na který ukazuje.
Použité třeba pro skoky.
```python
[source][dest] <-NAME
```	
## Adresa načítací instrukce
Toto vytvoří vyrtuální proměnou [NAME] která všude v kódu je nahrazena adresou instrukce "source".
Pouužité třeba pro načítání stringu z paměti. Přičítáním do [NAME] budeme totiž číst o registr dál
```python
[source][dest] {-NAME
```	
## Adresa ukládací instrukce
Toto vytvoří vyrtuální proměnou [NAME] která všude v kódu je nahrazena adresou instrukce "dest".
Pouužité třeba pro ukládání stringu do paměti. Přičítáním do [NAME] budeme totiž psát do registru dál
```python
[source][dest] }-NAME
```	
## Příklad nezkompilovaného kódu:
```python
#Příklad skoku
DATA: #informace kompileru kde jsou data (!nelze přehodit s code. musí být první!)

	tim 0	#čítač (aby se něco dělo)
	rtrn 69  #proměná do které uložíme kam se vracíme
	
CODE:	#informace kompileru kde začíná kód

	VAL_NULL VAL_NULL #zbytečný řádek, jeho provedení nic nezmění. používá se pro debug protože debuger ho zvýrazní
	
	tim MONITOR <-LOOP  #loop který jen počítá a zapisuje na MONITOR.(ukazovátko na instrukci zapsaání na monitor)

	PROG_NEXT rtrn #nasatvení navratu na na za funkc
	FUNKCE PROG_POS #zavolání funkce

	LOOP PROG_POS #konec loopu skočení na návěští LOOP

	VAL_NULL VAL_NULL #zbytečný řádek, jeho provedení nic nezmění. používá se pro debug protože debuger ho zvýrazní
	
	  tim ADD_B    <-FUNKCE #návěští volané funkce co přidá k TIM 1 (uložení do sčitaciho registu)
	  VAL_ONE ADD_A		#načtení 1 z registru obsaující 1
	  ADD_OUT tim		#výsledek načteme zpět do tim
	rtrn PROG_POS		vrácení z funkce
```
## Zkompilovaný kód(nedovoluje komentáře ty jous jen pro orientaci):
Paměťový prostor vypisujeme od 100 protože před stovkou jsou sčítací registry
```python
106 #PROG_POS registr, na prvním řádku je vždy adresa první istrukce (také se zvyšuje s tím jak program poběží) [100]
106 #na začátku setjné jako PROG_POS ale nezvyšuje se když se skáče, je dobré na restart programu               [101]
108 #adresa návěští LOOP											[102]
118 #adresa návěští FUNKCE											[103]
0    #proměná tim												[104]
69   #proměná rtrn												[105]
0    #první instrukce načti                               							[106]
0    #první instrukce vlož (tyto dvě instrukce jsou náš zbytečný řádek)                              		[107]
104  #načtení z tim          		                   	               					[108]
48   #vložení na monitor      		                     	               					[109]
51   #načtení z [PROG_NEXT]											[110]
105  #vložení na adresu promené [rtrn]										[111]
103  #načtení z návěší(proměné) [FUNKCE]									[112]
100  #vložení do PROG_POS čímž skočí na řádek obsažený v [FUNKCE] (kód pokračuje na řádku [118])		[113]
102  #načtení návěští LOOP											[114]
100  #vložení vložení do PROG_POS čímž skočí na řádek obsažený v [FUNKCE] (kód pokračuje na řádku [108])	[115]
0    #načtení nuly												[116]
0    #vložení vložení do registru nuly (tyto dvě instrukce jsou náš druhý zbytečný řádek)			[117]
104  #načtení obsahu proměné [tim]										[118]
11   #vložení do sčítacího registru [ADD_B]									[119]
1    #načtení načtení 1 z [VAL_ONE]										[120]
10   #vložení do sčítacího registru [ADD_A]									[121]
12   #načtení vysledku z sčítacího registru [ADD_OUT]								[122]
104  #vložení do rpoměné [tim]											[123]
105  #načtení načtení proměné [rtrn] kam jsem si uložili kam se program má vratit				[124]
100  #vložení do PROG_POS čímž skočí na řádek obsažený v [FUNKCE] (kód pokračuje na řádku [102])		[125]

```
## Zkompilovaný kód(zvýraznený barvamy v interpretu):
![IDK](https://user-images.githubusercontent.com/59420562/230725176-319d7a69-c20e-4b12-834d-8fa500fcd93f.png)



# Kompilátor:
Soubor compiler.py je kompilátor který z move kódu udělá číselný kód čitelný interpretem.
Interpret automaticky při příkazu load zkompiluje soubor code.base na code.code který pak načítá.
To ale uživatel může udělat sám (třeba při poruše souboru code.code) si může zkompilovat a nahradit code.code sám
Kompilátor umžnuje používat makra a spojovat víe souboru

Příklad makra je v examples/macro_use.base
> :warning: makra hlavně ve spojených souborech občas dělají binec.

#help kompilátoru:

![image](https://user-images.githubusercontent.com/59420562/234114975-bb290131-7052-49b4-8cab-f4974a84c9cf.png)
```python
INCLUDE library.base  # vložení souboru. jakoby spojí soubory. Zatím lze pouze psát v hlavním souboru

MACRO CALL:  # definice makra CALL , CALL se všude v kódu nahradí tímto
	PROG_NEXT RTRN #nasatvení navratu na na za funkcí
	$0 PROG_POS #zavolání funkce. $0 je prní parametr CALL
MACRO_END	# konec makra
 

MACRO RETURN:  
	
MACRO_END

DATA:
	TIM 0	#proměná
	RTRN 0  #proměná do které uložíme kam se vracíme
	POLE  1 #první položka pole. Proměné jou do paměti zapisovány za sebou
	- 2		#další položka pole. Tudíž víme že tato bude hned za začátkem
	- 3		#další položka pole. Compiler - nepovažuje za opetovnou deklarace proměné
	- 4
	- 5
CODE:

	VAL_NULL  VAL_NULL <-LOOP #zbytečný řádek, nic nedělá protože zapíše z 0 do 0 tudíš se nic nezmění

	CALL FUNKCE # použití makra na zavolání funkce

	TIM MONITOR }-DESTINACE # všude v kódu se nahradí "DESTINACE" za dresu registru MONITOR

	CODE PROG_POS # CODE je ukazovátko na začátek programu (prví CODE:)

	VAL_NULL VAL_NULL
	  TIM ADD_B    <-FUNKCE # Ukazovátko vytvoří proměnou obsahující adresu na instrukci na stejném řádku 
	  VAL_ONE ADD_A
	  ADD_OUT TIM 		{-ZDROJ # všude v kódu se nahradí "ZDROJ" za dresu registru ADD_OUT
	RTRN PROG_POS    #vrácení se na místo odkud jsme funkci volali
FILE_END:		#Toto je dobrovolé, kompilátor si toto sám umí doplnit
```

# Struktura Jazyka MOVe(Compi 1.02):
```python
[FUNKČNÍ REGISTRY] #tady se nachází registry jako ADD_A,ADD_B a další 
	...
	...
[FUNKČNÍ REGISTRY]

[PROGRAM_COUNTER] #na první programové adrese je vždy registr další instrukce k proedení
[PROGRAM_LENNGHT] #na druhé programové adrese konec prostoru proměných a tudíž začátek kódu. tento register se jmenuje CODE protože v něm je uložená adresa první pozice 
	
[PROGRAM_JUMP_ADRESSES] # zde se nachází adresy na návěští které v programu použijeme
	...
[PROGRAM_JUMP_ADRESSES]
	
[PROGRAM_VARIABLES]		# zde se anchází proměné
	...
[PROGRAM_VARIABLES]
	
[PROGRAM_INSTRUCTIONS]	# zde se nachází instrukce, vždy po dvojicích
	...
[PROGRAM_INSTRUCTIONS]
```
# Konečné slovo:
> :information_source: Projekt i když není scela odladěný je opuštený. Vše popsané v tomto souboru je funkční ale neplánuji v blízké budoucnosti přidávat nové funkce. 
