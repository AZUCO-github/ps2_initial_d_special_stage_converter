///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		FPK format
//			061023
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>

extern	bool	is_icp		(unsigned char *,char *);
extern	bool	is_bin		(unsigned char *,char *);
		bool	is_fpk		(unsigned char *,char *);
extern	void	decode_icp	(unsigned char *,unsigned long,char *);
extern	void	decode_bin	(unsigned char *,char *,unsigned long);
		void	decode_fpk	(char *,unsigned long,unsigned char *);



/*****メモ*****FPK フォーマット************************************************
0x00		アイテムテーブル(0x18 x n)
0x??+0x00	ファイル名（すべて0の場合アイテムテーブルの終端）
0x??+0x10	アイテムのアドレス>>4
0x??+0x14	アイテムの長さ
******************************************************************************/



void	decode_fpk(char *name,unsigned long slen,unsigned char *src)
{
	unsigned long	INO;		//Item NO.
	unsigned long	IDA;		//Item Data Address
	unsigned long	ILN;		//Item LeNgth

	char			INB[0x20];	//Item Name Buffer

	char			drive[MAX_PATH];
	char			dir[MAX_PATH];
	char			fname[MAX_PATH];
	char			ext[MAX_PATH];


	for(INO=0;;INO++){
		IDA=(*(unsigned long*)(src+0x18*INO+0x10))<<4;
		ILN= *(unsigned long*)(src+0x18*INO+0x14);

		memcpy(INB,src+0x18*INO,0x10);
		INB[0x11]=0x00;
		if(INB[0x00]==0)	break;								//終端判定

		_splitpath(INB,drive,dir,fname,ext);

printf("%s > No.%04X %08X %s\n",name,INO,ILN,INB);

		if(strcmp(_strlwr(ext),".icp")==0){
			decode_icp(src+IDA,ILN,fname);
			goto CONV_DONE;						}

			decode_bin(	src+IDA,INB,ILN);
CONV_DONE:
		NULL;
	}
}



bool	is_fpk(unsigned char *src,char *name)
{	if(strstr(name,".fpk")!=NULL)	return(TRUE);
	if(strstr(name,".FPK")!=NULL)	return(TRUE);
									return(FALSE);	}
