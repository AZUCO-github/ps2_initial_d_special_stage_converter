///////////////////////////////////////////////////////////////////////////////
//	AFS converter
//		040723
//			by	AZUCO
///////////////////////////////////////////////////////////////////////////////
#include	<windows.h>
#include	<stdio.h>

extern	bool	is_tim2		(unsigned char *);
extern	bool	is_pac		(unsigned char *);
extern	bool	is_gim		(unsigned char *);
extern	void	decode_tim2	(char *,unsigned long,unsigned char *);
extern	void	decode_pac	(char *,unsigned long,unsigned char *);
extern	void	decode_gim	(char *,unsigned long,unsigned char *);
extern	void	decode_fpk	(char *,unsigned long,unsigned char *);



void	main(void)
{	char			file[_MAX_PATH];
	char			*tfpa;
	char			*tfpb;
	unsigned char	*buff=NULL;
	unsigned long	file_size;
	HANDLE			hndl;
	unsigned long	tul;
	char			drive[MAX_PATH];
	char			dir[MAX_PATH];
	char			fname[MAX_PATH];
	char			ext[MAX_PATH];

	printf("PS2 InitialD Special Stage Converter\n");
	printf("\t\tProg. By AZUCO\n\n");



	strcpy(file,GetCommandLine());				//コマンドラインをコピーする

	tfpa=strrchr(file,' ');		//一番最後のスペース以降をファイル名と見なす
	if(tfpa==NULL){
		printf("ps2idss <file>\n");
		printf("\tError\tsource file not found\n");
		exit(-1);	}
	tfpa++;

	tfpb=strrchr(tfpa,'"');		//ダブルクオートありの場合
	if(tfpb!=NULL)	*tfpb=0;

	tfpb=strrchr(tfpa,'>');		//リダイレクトの場合
	if(tfpb!=NULL)	*tfpb=0;



	tfpb=&file[0];
loop:
	if(*tfpa=='\\'){	*(tfpb+0)='\\';
						*(tfpb+1)='\\';
						tfpa++;
						tfpb+=2;
						goto loop;		}
	if(*tfpa>=0x20){	*tfpb=*tfpa;
						tfpa++;
						tfpb++;
						goto loop;		}
						*tfpb=0;



//strcpy(file,"E:\\TMP\\RIOPARA\\RIP2_MOV.AFS");
//printf("filename=%s\n",file);

	_splitpath(file,drive,dir,fname,ext);

	hndl=CreateFile(file,
					GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(hndl==(HANDLE)-1){
		printf("ps2idss <file>\n");
		printf("\tError\tsource file not found\n");
		exit(-1);	}

	file_size=GetFileSize(hndl,NULL);

	buff=new unsigned char[file_size];
	ReadFile(hndl,buff,file_size,&tul,NULL);

	if(is_pac (buff)){
		decode_pac (fname,file_size  ,buff  );	goto esc;	}
	if(	strcmp(_strlwr(ext),".pac")==0){
		decode_pac(fname,file_size  ,buff  );	goto esc;	}

	if(is_gim (buff)){
		decode_gim (fname,file_size  ,buff  );	goto esc;	}
	if(	strcmp(_strlwr(ext),".gim")==0){
		decode_gim(fname,file_size  ,buff  );	goto esc;	}

	if(is_tim2(buff)){
		decode_tim2(fname,file_size  ,buff  );	goto esc;	}
	if(is_tim2(buff+2)){
		decode_tim2(fname,file_size-2,buff+2);	goto esc;	}
	if(	strcmp(_strlwr(ext),".tm2")==0){
		decode_tim2(fname,file_size  ,buff  );	goto esc;	}

	if(	strcmp(_strlwr(ext),".fpk")==0){
		decode_fpk(fname,file_size  ,buff  );	goto esc;	}
esc:
	CloseHandle(hndl);
	if(buff!=NULL)	delete[]	buff;
}
