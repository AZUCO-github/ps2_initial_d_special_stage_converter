///////////////////////////////////////////////////////////////////////////////
//	PS2 “ª•¶ŽšD Special Stage
//		ICP format
//			061023
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);

extern	HANDLE	hbmp;
extern	void	tim2_uncompress(	unsigned char *,	unsigned char *,
									unsigned long,		unsigned long);
extern	bool	is_tim2		(unsigned char *);
extern	void	decode_tim2(char *,unsigned long,unsigned char *);
		void	decode_icp(unsigned char *,unsigned long,char *);

void	decode_icp(unsigned char *sbuf,unsigned long olen,char *fname)
{
	unsigned long		val_1=*(unsigned long*)(sbuf+0);
	unsigned long		dlen=*(unsigned long*)(sbuf+4);
	unsigned char		*dbuf;
	char				name[MAX_PATH];
	HANDLE				dhdl;
	unsigned long		tul1;

	dbuf=new unsigned char[dlen+0x1000];
	tim2_uncompress(sbuf+8,dbuf,olen-8,dlen);
	if(is_tim2(dbuf))
			decode_tim2(fname,dlen,dbuf);
	else{	sprintf(name,"%s.bin",fname);
			dhdl=NonOverWriteCreateFile((LPCSTR)name);
			WriteFile(dhdl,dbuf,dlen,&tul1,NULL);
			CloseHandle(dhdl);
	}
	delete[]	dbuf;
}
