///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		PAC format
//			030909
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>

		bool	is_cmd			(unsigned char *);
extern	bool	is_gim			(unsigned char *);
extern	bool	is_bin			(unsigned char *);
extern	bool	is_123dda01		(unsigned char *);
		bool	is_pac			(unsigned char *);
extern	void	decode_gim		(char *,unsigned long,unsigned char *);
extern	void	decode_bin		(unsigned char *,char *,unsigned long);
		void	decode_pac		(char *,unsigned long,unsigned char *);



void	decode_pac(char *name,unsigned long slen,unsigned char *src)
{	unsigned long	INO;
	unsigned long	IMX=*(unsigned long*)(src+4);
	unsigned long	IDA;
	unsigned long	ILN;

	printf("\tPAC\t%s\n",(char *)src+0x10);

	for(INO=0;INO<IMX;INO++){
		IDA=*(unsigned long*)(src+0x20+INO*0x20+0x10);
		ILN=*(unsigned long*)(src+0x20+INO*0x20+0x14);

		if(is_gim			(src+IDA)){
			decode_gim		((char *)src+0x20+INO*0x20,ILN,src+IDA);
			goto CONV_DONE;	}
		if(is_cmd			(src+IDA)){
			goto CONV_DONE;	}

		decode_bin			(src+IDA,(char *)src+0x20+INO*0x20,ILN);

CONV_DONE:
		NULL;
	}
}



bool	is_pac(unsigned char *src)
{	if(	(src[0]=='P')&&
		(src[1]=='A')&&
		(src[2]=='C')	)	return(TRUE);
							return(FALSE);
}



bool	is_cmd(unsigned char *src)
{	if(	(src[0]=='C')&&
		(src[1]=='M')&&
		(src[2]=='D')	)	return(TRUE);
							return(FALSE);
}


