///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		GIM format
//			030909
//				by	AZUCO
///////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<stdio.h>

extern	void	decode_palette_8bit(unsigned char *);
extern	void	decode_palette_4bit(unsigned char *);

extern	HANDLE	NonOverWriteCreateFile(LPCSTR);

		bool	is_gim					(unsigned char *);
		void	decode_gim				(char *,unsigned long,unsigned char *);
		void	decode_gim_4bit			(unsigned char *,char *);
		void	decode_gim_4bit_encode	(unsigned char *,char *);
		void	decode_gim_8bit			(unsigned char *,char *);
		void	decode_gim_8bit_encode	(unsigned char *,char *);

void	decode_gim(char *name,unsigned long slen,unsigned char *src)
{
	switch(*(src+0x34)&0x03){
		case	0x00:
			decode_gim_4bit			(src,name);
			break;
		case	0x01:
			printf("GIM TYPE 01\n");	break;
		case	0x02:
			printf("GIM TYPE 02\n");	break;
		case	0x03:
			decode_gim_8bit_encode	(src,name);
			break;
	}
/*
	//Check color depth		0x02=4bit / 0x04=8bit
	if(*(src+0x4e)==0x02){	decode_gim_4bit(src,name);	return;	}
	if(*(src+0x4e)==0x00){	decode_gim_4bit(src,name);	return;	}
	//Check encode			0x02=no   / 0x01=encode
	if(*(src+0x37)==0x01){	decode_gim_8bit(src,name);	return;	}
							decode_gim_8bit_encode(src,name);
*/
}



void	decode_gim_8bit_encode(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz;
	unsigned long		ax,ay;
	unsigned long		gp;
	unsigned char		dst[0x40000];
	char				tfn[0x100];
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;
	HANDLE				hbmp;

	printf("\tGIM 8bit enc\t%s\n",name);

	sx=*(unsigned short*)(src+0x30);
	sy=*(unsigned short*)(src+0x32);
	gp=*(unsigned short*)(src+0x3c);
	sz=sx*sy;

	strcpy(tfn,name);
	strcat(tfn,".bmp");
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x400+						//palette
						sz;							//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x400;						//palette

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		8;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	src+=gp;

	decode_palette_8bit(src+sz);
	WriteFile(hbmp,src+sz,256*4,&tul1,NULL);

	for(ax=0;ax<sz;ax+=16){
		dst[ax+ 0]=src[ax+0+ 0];
		dst[ax+ 1]=src[ax+0+ 4];
		dst[ax+ 2]=src[ax+0+ 8];
		dst[ax+ 3]=src[ax+0+12];
		dst[ax+ 4]=src[ax+1+ 0];
		dst[ax+ 5]=src[ax+1+ 4];
		dst[ax+ 6]=src[ax+1+ 8];
		dst[ax+ 7]=src[ax+1+12];
		dst[ax+ 8]=src[ax+2+ 0];
		dst[ax+ 9]=src[ax+2+ 4];
		dst[ax+10]=src[ax+2+ 8];
		dst[ax+11]=src[ax+2+12];
		dst[ax+12]=src[ax+3+ 0];
		dst[ax+13]=src[ax+3+ 4];
		dst[ax+14]=src[ax+3+ 8];
		dst[ax+15]=src[ax+3+12];
	}

	for(ay=0;ay<sz;ay+=sx*8){
		for(ax=0;ax<(sx*4);ax+=8){
			memcpy(&src[ay+ax+0],&dst[ay+ax+4],4);
			memcpy(&src[ay+ax+4],&dst[ay+ax+0],4);		}
			memcpy(&src[ay+sx*4],&dst[ay+sx*4],sx*4);
	}

	for(ay=0;ay<sz;ay+=0x20){
			memcpy(&dst[ay+(0x00<<2)],&src[ay+(0x04<<2)],4);
			memcpy(&dst[ay+(0x01<<2)],&src[ay+(0x05<<2)],4);
			memcpy(&dst[ay+(0x02<<2)],&src[ay+(0x06<<2)],4);
			memcpy(&dst[ay+(0x03<<2)],&src[ay+(0x07<<2)],4);

			memcpy(&dst[ay+(0x04<<2)],&src[ay+(0x01<<2)],4);
			memcpy(&dst[ay+(0x05<<2)],&src[ay+(0x00<<2)],4);
			memcpy(&dst[ay+(0x06<<2)],&src[ay+(0x03<<2)],4);
			memcpy(&dst[ay+(0x07<<2)],&src[ay+(0x02<<2)],4);
	}

	for(ay=0;ay<sz;ay+=sx*2){
		for(ax=0;ax<sx*2;ax+=0x20){
			memcpy(&src[ay+ 0+(ax>>1)],&dst[ay+ 0+ax+0x00],0x10);
			memcpy(&src[ay+sx+(ax>>1)],&dst[ay+ 0+ax+0x10],0x10);
		}
	}

	for(ay=0;ay<sz;ay+=sx*2){
		for(ax=0;ax<sx;ax+=8){
			memcpy(&dst[ay+ 0+ax+0],&src[ay+sx+ax+0],4);
			memcpy(&dst[ay+sx+ax+0],&src[ay+ 0+ax+0],4);
			memcpy(&dst[ay+ 0+ax+4],&src[ay+ 0+ax+4],4);
			memcpy(&dst[ay+sx+ax+4],&src[ay+sx+ax+4],4);
		}
	}

	for(ax=0;ax<sz;ax+=sx*8){
		memcpy(&src[ax+sx*0],&dst[ax+sx*0],sx);
		memcpy(&src[ax+sx*1],&dst[ax+sx*2],sx);
		memcpy(&src[ax+sx*2],&dst[ax+sx*1],sx);
		memcpy(&src[ax+sx*3],&dst[ax+sx*3],sx);

		memcpy(&src[ax+sx*4],&dst[ax+sx*5],sx);
		memcpy(&src[ax+sx*5],&dst[ax+sx*7],sx);
		memcpy(&src[ax+sx*6],&dst[ax+sx*4],sx);
		memcpy(&src[ax+sx*7],&dst[ax+sx*6],sx);
	}

	for(tul2=sz;tul2<=sz;tul2-=sx)
		WriteFile(hbmp,src+tul2,sx,&tul1,NULL);

	CloseHandle(hbmp);
}



void	decode_gim_8bit(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz;
	unsigned long		gp;
	char				tfn[0x100];
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;
	HANDLE				hbmp;



	printf("\tGIM 8bit\t%s\n",name);

	sx=*(unsigned short*)(src+0x30);
	sy=*(unsigned short*)(src+0x32);
	gp=*(unsigned short*)(src+0x3c);
	sz=sx*sy;

	strcpy(tfn,name);
	strcat(tfn,".bmp");
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x400+						//palette
						sz;					//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x400;						//palette

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		8;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	decode_palette_8bit(src+gp+sz);
	WriteFile(hbmp,src+gp+sz,256*4,&tul1,NULL);

//ƒxƒ^‚È‚Ì‚ÅA‚Ð‚Á‚­‚è•Ô‚·‚¾‚¯‚Å—Ç‚¢
	for(tul2=sz;tul2<=sz;tul2-=0x200)
		WriteFile(hbmp,src+gp+tul2,0x200,&tul1,NULL);

	CloseHandle(hbmp);
}



void	decode_gim_4bit_encode(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz;
	unsigned long		ax,ay;
	unsigned long		gp;
	unsigned char		dst[0x40000];
	char				tfn[0x100];
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;
	HANDLE				hbmp;



	printf("\tGIM 4bit enc\t%s\n",name);

	sx=*(unsigned short*)(src+0x30);
	sy=*(unsigned short*)(src+0x32);
	gp=*(unsigned short*)(src+0x3c);
	sz=sx*sy/2;

	strcpy(tfn,name);
	strcat(tfn,".bmp");
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x40+						//palette
						sz;							//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x40;						//palette

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		4;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	src+=gp;

	decode_palette_4bit(src+sz);
	WriteFile(hbmp,src+sz,16*4,&tul1,NULL);

	for(tul2=0;tul2<sz;tul2++){
		tul1=src[tul2];
		src[tul2]=((tul1&0x0f)<<4)|((tul1&0xf0)>>4);	}



	for(ax=0;ax<sz;ax+=16){
		dst[ax+ 0]=src[ax+0+ 0];
		dst[ax+ 1]=src[ax+0+ 4];
		dst[ax+ 2]=src[ax+0+ 8];
		dst[ax+ 3]=src[ax+0+12];
		dst[ax+ 4]=src[ax+1+ 0];
		dst[ax+ 5]=src[ax+1+ 4];
		dst[ax+ 6]=src[ax+1+ 8];
		dst[ax+ 7]=src[ax+1+12];
		dst[ax+ 8]=src[ax+2+ 0];
		dst[ax+ 9]=src[ax+2+ 4];
		dst[ax+10]=src[ax+2+ 8];
		dst[ax+11]=src[ax+2+12];
		dst[ax+12]=src[ax+3+ 0];
		dst[ax+13]=src[ax+3+ 4];
		dst[ax+14]=src[ax+3+ 8];
		dst[ax+15]=src[ax+3+12];
	}

	for(ay=0;ay<sz;ay+=sx*8){
		for(ax=0;ax<(sx*4);ax+=8){
			memcpy(&src[ay+ax+0],&dst[ay+ax+4],4);
			memcpy(&src[ay+ax+4],&dst[ay+ax+0],4);		}
			memcpy(&src[ay+sx*4],&dst[ay+sx*4],sx*4);
	}

	for(ay=0;ay<sz;ay+=0x20){
			memcpy(&dst[ay+(0x00<<2)],&src[ay+(0x04<<2)],4);
			memcpy(&dst[ay+(0x01<<2)],&src[ay+(0x05<<2)],4);
			memcpy(&dst[ay+(0x02<<2)],&src[ay+(0x06<<2)],4);
			memcpy(&dst[ay+(0x03<<2)],&src[ay+(0x07<<2)],4);

			memcpy(&dst[ay+(0x04<<2)],&src[ay+(0x01<<2)],4);
			memcpy(&dst[ay+(0x05<<2)],&src[ay+(0x00<<2)],4);
			memcpy(&dst[ay+(0x06<<2)],&src[ay+(0x03<<2)],4);
			memcpy(&dst[ay+(0x07<<2)],&src[ay+(0x02<<2)],4);
	}

	for(ay=0;ay<sz;ay+=sx*2){
		for(ax=0;ax<sx*2;ax+=0x20){
			memcpy(&src[ay+ 0+(ax>>1)],&dst[ay+ 0+ax+0x00],0x10);
			memcpy(&src[ay+sx+(ax>>1)],&dst[ay+ 0+ax+0x10],0x10);
		}
	}

	for(ay=0;ay<sz;ay+=sx*2){
		for(ax=0;ax<sx;ax+=8){
			memcpy(&dst[ay+ 0+ax+0],&src[ay+sx+ax+0],4);
			memcpy(&dst[ay+sx+ax+0],&src[ay+ 0+ax+0],4);
			memcpy(&dst[ay+ 0+ax+4],&src[ay+ 0+ax+4],4);
			memcpy(&dst[ay+sx+ax+4],&src[ay+sx+ax+4],4);
		}
	}

	for(ax=0;ax<sz;ax+=sx*8){
		memcpy(&src[ax+sx*0],&dst[ax+sx*0],sx);
		memcpy(&src[ax+sx*1],&dst[ax+sx*2],sx);
		memcpy(&src[ax+sx*2],&dst[ax+sx*1],sx);
		memcpy(&src[ax+sx*3],&dst[ax+sx*3],sx);

		memcpy(&src[ax+sx*4],&dst[ax+sx*5],sx);
		memcpy(&src[ax+sx*5],&dst[ax+sx*7],sx);
		memcpy(&src[ax+sx*6],&dst[ax+sx*4],sx);
		memcpy(&src[ax+sx*7],&dst[ax+sx*6],sx);
	}

//		memcpy(src,dst,sz);



	for(tul2=(sz-(sx>>1));tul2<=sz;tul2-=(sx>>1))
		WriteFile(hbmp,src+gp+tul2,(sx>>1),&tul1,NULL);

	CloseHandle(hbmp);
}



void	decode_gim_4bit(unsigned char *src,char *name)
{	unsigned long		tul1,tul2;
	unsigned long		sx,sy,sz;
	unsigned long		gp;
	char				tfn[0x100];
	BITMAPFILEHEADER	bf;
	BITMAPINFOHEADER	bi;
	HANDLE				hbmp;



	printf("\tGIM 4bit\t%s\n",name);

	sx=*(unsigned short*)(src+0x30);
	sy=*(unsigned short*)(src+0x32);
	gp=*(unsigned short*)(src+0x3c);
	sz=sx*sy/2;

	strcpy(tfn,name);
	strcat(tfn,".bmp");
	hbmp=NonOverWriteCreateFile((LPCSTR)tfn);
	if(hbmp==(HANDLE)-1){
		printf("\tError\tcan't create bmp file\n");
		exit(-1);	}



	*((char *)&bf.bfType+0)='B';
	*((char *)&bf.bfType+1)='M';
	bf.bfSize=			sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x40+						//palette
						sz;							//image
	bf.bfOffBits=		sizeof(BITMAPFILEHEADER)+
						sizeof(BITMAPINFOHEADER)+
						0x40;						//palette

	bi.biSize=			sizeof(BITMAPINFOHEADER);
	bi.biWidth=			sx;
	bi.biHeight=		sy;
	bi.biPlanes=		1;
	bi.biBitCount=		4;
	bi.biCompression=	0;
	bi.biSizeImage=		0;
	bi.biXPelsPerMeter=	0;
	bi.biYPelsPerMeter=	0;
	bi.biClrUsed=		0;
	bi.biClrImportant=	0;

	WriteFile(hbmp,&bf,sizeof(BITMAPFILEHEADER),&tul1,NULL);
	WriteFile(hbmp,&bi,sizeof(BITMAPINFOHEADER),&tul1,NULL);

	decode_palette_4bit(src+gp+sz);
	WriteFile(hbmp,src+gp+sz,16*4,&tul1,NULL);

	for(tul2=0;tul2<sz;tul2++){
		tul1=src[gp+tul2];
		src[gp+tul2]=((tul1&0x0f)<<4)|((tul1&0xf0)>>4);	}

	for(tul2=(sz-(sx>>1));tul2<=sz;tul2-=(sx>>1))
		WriteFile(hbmp,src+gp+tul2,(sx>>1),&tul1,NULL);

	CloseHandle(hbmp);
}



bool	is_gim(unsigned char *src)
{	if(	(src[0]=='G')&&
		(src[1]=='I')&&
		(src[2]=='M')	)	return(TRUE);
							return(FALSE);
}



