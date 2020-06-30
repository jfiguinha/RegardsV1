#include <math.h>
#include <omp.h>
#include <xmmintrin.h>

static __m64 Get_m64(__int64 n);

//////////////////////////////////////////////////////////
//Transforme des données sur 16 bits sur 8 bits
//////////////////////////////////////////////////////////

static void fonct16to8(void * kernel, void * data)
{
	_asm
	{
		;;conversion des données sur 8 bit
		mov esi, kernel 
		mov edi, data
		
		pxor mm2, mm2

		movq mm1, 0[ESI]  // Read in source data 

		PACKUSWB mm1, mm2

		movq 0[EDI], mm1

		emms

	}
}

////////////////////////////////////////////////////////////////////////////////
//Additionne et multiplie 3 opérandes -> données en entrée doivent être en short
////////////////////////////////////////////////////////////////////////////////

static void muladdfilter(void * kernel, void * data, void * dst)
{
	_asm { 

		mov esi, kernel 
		mov edi, data 

		movq mm1, 0[ESI]  // Read in source data 
		movq mm2, 0[EDI]
	
		movq mm3, 8[ESI]  // Read in source data 
		movq mm4, 8[EDI]

		movq mm5, 16[ESI]  // Read in source data 
		movq mm6, 16[EDI]
		
		mov esi, dst

		movq mm7, 0[ESI]

		;;pxor mm7, mm7 ;;Mise à zéro des données

		;;Multiplication en unsigned

		pmullw mm1, mm2
		pmullw mm3, mm4
		pmullw mm5, mm6

		paddusw mm7,mm1
		paddusw mm7,mm3
		paddusw mm7,mm5

		

		;;Enregistrement des données sur 16 bits

		movq 0[ESI], mm7

		emms 
	} 
}

////////////////////////////////////////////////////////////////////////////////
//Additionne et multiplie 4 opérandes -> données en entrée doivent être en short
////////////////////////////////////////////////////////////////////////////////

static void mulShortfilter(void * kernel, void * data, void * dst)
{
	_asm { 

			
			mov edi, kernel
			movq mm0, 0[EDI]

			mov esi, data
			
			movq mm1, 0[ESI]  // Read in source data 
			movq mm2, 8[ESI]  // Read in source data 
			movq mm3, 16[ESI]  // Read in source data 
			movq mm4, 24[ESI]  // Read in source data

			;;Multiplication en unsigned

			pmullw mm1, mm0
			pmullw mm2, mm0
			pmullw mm3, mm0
			pmullw mm4, mm0

			;;Enregistrement des données sur 16 bits

			mov edi, dst
			movq 0[EDI], mm1
			movq 8[EDI], mm2
			movq 16[EDI], mm3
			movq 24[EDI], mm4

			emms 
	} 
}

////////////////////////////////////////////////////////////////////////////////
//Additionne et multiplie 3 opérandes -> données en entrée doivent être en short
////////////////////////////////////////////////////////////////////////////////

static void mulByteFilter(void * kernel, void * data, void * dst, int nbytes)
{
	_asm { 

		mov edi, kernel 
		movq mm0, 0[EDI]

		mov edi, dst
		mov esi, data 

		mov ecx, nbytes 
		shr ecx, 4 // 16 bytes per iteration 

loop1:

		movq mm1, 0[ESI]  // Read in source data 
		movq mm2, 8[ESI]
		movq mm3,16[ESI]  // Read in source data 
		movq mm4,24[ESI]

		;;Multiplication en unsigned

		pmullw mm1, mm0
		pmullw mm2, mm0
		pmullw mm3, mm0
		pmullw mm4, mm0

		;;Enregistrement des données sur 16 bits

		movq  0[EDI],mm1  // Read in source data 
		movq  8[EDI],mm2
		movq 16[EDI],mm3  // Read in source data 
		movq 24[EDI],mm4

		add esi, 32
		add edi, 32
		dec ecx 
		jnz loop1

		emms 
	} 
}

////////////////////////////////////////////////////////////////////////////////
//Additionne et multiplie 3 opérandes -> données en entrée doivent être en short
////////////////////////////////////////////////////////////////////////////////

static void addfilter(void * data, void * dst)
{
	_asm { 

		mov edi, data 
		mov esi, dst

		movq mm1, 0[EDI]  // Read in source data 
		movq mm2, 8[EDI]
		movq mm3,16[EDI]  // Read in source data 

		movq mm7, 0[ESI]

		paddusw mm7,mm1
		paddusw mm7,mm2
		paddusw mm7,mm3	

		;;Enregistrement des données sur 16 bits

		movq 0[ESI], mm7

		emms 
	} 
}


static void addConvert(void * data, void * koffset,void * result)
{
	_asm { 

		mov edi, data 
		mov esi, koffset

		movq mm1, 0[EDI]  // Read in source data 
		movq mm7, 0[ESI]

		paddusw mm7,mm1

		mov esi, result

		;;Enregistrement des données sur 16 bits

		PACKUSWB mm1, mm7

		movq 0[ESI], mm1

		emms 
	} 
}

//////////////////////////////////////////////////////////
//Transforme des données sur 16 bits sur 8 bits
//////////////////////////////////////////////////////////

static void addConvertDiv(void * data, void * koffset,void * result)
{
	_asm
	{
		;;conversion des données sur 8 bit
		mov esi, data 
		mov edi, koffset
		
		movq mm1, 0[ESI]  // Read in source data 
		movq mm7, 0[EDI]  // Read in source data 		

		PSRLW mm1, 8 ;;Division par 256

		mov esi, result

		paddusw mm1,mm7

		PACKUSWB mm1, mm7

		movq 0[ESI], mm1

		emms

	}
}

//Effet Negatif

__m64 Get_m64(__int64 n)
{
    union __m64__m64
    {
        __m64 m;
        __int64 i;
    } mi;

    mi.i = n;
    return mi.m;
}