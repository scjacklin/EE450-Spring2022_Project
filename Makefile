all: header.h serverM.c serverA.c serverB.c serverC.c clientA.c clientB.c
	gcc serverM.c -o serverM
	gcc serverA.c -o serverA 
	gcc serverB.c -o serverB
	gcc serverC.c -o serverC
	gcc clientA.c -o clientA
	gcc clientB.c -o clientB

