# Toto je interpret a debugovací program pro MOVe 
![Snímek obrazovky 2023-04-08 142912](https://user-images.githubusercontent.com/59420562/230721129-a7f1dd1f-1905-463d-9922-a4c3632c4acd.png)
# Co interpret umožňuje?
Umožňuje náhled na která instrukce se v kódu provádí.
Má příkazi na debug:

		run -> spustí simulaci
		stop -> zastaví simulaci
		key -> připojí klávesnici na vstup
	
		hvar -> zvírazní co adresi proměných
		hvare -> zvírazní konec proměnného prostoru 
		hjmp -> zvýrazní zápis do pozicového registru
		hall -> zapne všechno zvýraznování
		clsr -> překreslí UI konzole.                   // občas při zvětšování dělá konzole zmatky

		step     -> provede jednu instrukci
		clock xx -> nastaví rychlost hodin použitých pokud program běží
		peek xx  -> zobrazí co register obsahuje
		mv AAA BBB -> přesune obsah z registru AAA do registru BBB // v debug verzi je třeba zadávat jako mv001025 // 1 >> 25
		
		load     -> compile and load program
		open     -> open program file

# Co je MOVe?
MOVe je programovací jazyk obsahující jen jedinou instrukci a to přesuň z adresy na adresu.

# Jak se v MOVe například sčítá když umí jen přesouvat registry?
Procesor má mnoho registrů které zvládají jednoduché operace.
Například:

	  MOJE_CISLO ADD_A
	  TVOJE_CISLO ADD_B
	  ADD_OUT MONITOR

Tento kus kódu provede sečtení čísla v registru MOJE_CISLO a čísla TVOJE_CISLO,
které zapíše na register MONITOR (viditelný v debugeru).

## komentáře v jazyce MOVe:
Vše o je za # je považované za komentář

# Registry jazyka MOVe:

        Konstanty:
                [VAL_NULL]    obsahuje 0
	        [VAL_ONE]     obsahuje 1
		[VAL_TWO]     obsahuje 2
		[VAL_FOUR]    obsahuje 4
		[VAL_SIX]     obsahuje 6
		[VAL_EIGHT]   obsahuje 8
		[VAL_SIXTEEN] obsahuje 16 
		[VAL_NEG]     obsahuje -1

		[RAND]        obsahuje náhodné číslo


        Speciální Registry: 
		[PROG_START]
		[PROG_POS]        obsahuje adresu dalšího příkazu
		[PROG_NEXT]	  obsahuje adresu příkazu po 2 provedení
		[PROG_NEXT_TWO]   obsahuje adresu příkazu po 3 provedení
		[PROG_NEXT_THREE] obsahuje adresu příkazu po 4 provedení
		[PROG_NEXT_FOUR]  obsahuje adresu příkazu po 5 provedení

		[G_POINTER]       register určený pro ukládání ukazování do paměi // !nedodělané
		[MAX_MEM]         obsahuje velikost maximální paměti
		[INTERUPT_JMP]    obsahuje adresu skoku při interuptu
		[INTERUPT_MODE]   obsahuje zapnutí interuptu. Reaguje na [DISPLAY1], 0 = vyplé, 1 = při změně na HIGH, 2 = při změně na LOW, 3 = při HIGH i LOW. 
		[PRE_INT_ADRESS]  obsahuje adresu ze kterého se do přerušení skočilo

		[MONITOR]	  register napojený na monitor v simulátoru
		
		[OUTPUT1]
		[OUTPUT2]
		[OUTPUT3]

		[INPUT1]
		[INPUT2]
		[INPUT3]

	Matematické Registry:
		[ADD_OUT] = [ADD_A] + [ADD_B]
		[SUB_OUT] = [SUB_A] - [SUB_B]
		[MUL_OUT] = [MUL_A] * [MUL_B]
		[DIV_OUT] = [DIV_A] / [DIV_B]
		[REM_OUT] = [REM_A] % [REM_B]
		[EQ_OUT]  = [EQ_A] == [EQ_B]
		[OR_OUT]  = [OR_A]  | [OR_B]
		[AND_OUT] = [AND_A] && [AND_B]

		[SHIFT_L_OUT] = [SHIFT_L_A] << 1
		[SHIFT_R_OUT] = [SHIFT_R_A] >> 1

		[SWITCH_OUT] = [SWITCH_S] ? [SWITCH_A] : [SWITCH_B]
		
		[NEG_OUT] = ~[NEG_A] 

# Ukazování na adresy v jazyka MOVe:

## Pozice řádku
toto vytvoří proměnou [NAME] obsahující číslo řádku na který ukazuje.
Použité třeba pro skoky.

	[source][dest] <-NAME
	
## Adresa načítací instrukce
Toto vytvoří vyrtuální proměnou [NAME] která všude v kódu je nahrazena adresou instrukce "source".
Pouužité třeba pro načítání stringu z paměti. Přičítáním do [NAME] budeme totiž číst o registr dál
	
	[source][dest] {-NAME
	
## Adresa ukládací instrukce
Toto vytvoří vyrtuální proměnou [NAME] která všude v kódu je nahrazena adresou instrukce "dest".
Pouužité třeba pro ukládání stringu do paměti. Přičítáním do [NAME] budeme totiž psát do registru dál

	[source][dest] }-NAME

# Struktura Jazyka MOVe(Compi 1.02):

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
