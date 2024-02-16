///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		bin
//			041206
//				by	AZUCO
//
//		AFS側では規定してないファイルなので単に切り出すのみ
//
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);

		bool	is_bin(unsigned char *,char *);
		void	decode_bin(unsigned char *,char *,unsigned long);



void	decode_bin(unsigned char *src,char *name,unsigned long length)
{	HANDLE			hbin;
	unsigned long	tul1;

	hbin=NonOverWriteCreateFile((LPCSTR)name);
	if(hbin==(HANDLE)-1){
		printf("\tError\tcan't create bin file\n");
		exit(-1);	}

	WriteFile(hbin,src,length,&tul1,NULL);
	CloseHandle(hbin);
}



bool	is_bin(unsigned char *src,char *name)
{
	if(strstr(name,".bin")!=NULL)	return(TRUE);
	if(strstr(name,".BIN")!=NULL)	return(TRUE);
	if(strstr(name,".amo")!=NULL)	return(TRUE);
	if(strstr(name,".AMO")!=NULL)	return(TRUE);
	if(strstr(name,".ahi")!=NULL)	return(TRUE);
	if(strstr(name,".AHI")!=NULL)	return(TRUE);
									return(FALSE);
}
