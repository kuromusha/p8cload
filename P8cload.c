/*
	    P8cload Ver.1.10

	Programmed by K.Kuromusha

	      Nov. 5, 1994
*/

#include	<stdio.h>

char	*fn1,*fn2,text[0x10000];
FILE	*fp1,*fp2;
int	ptr=0,p=0,flag=0,data,r=3,a,x=0,y=0,z=0,
	hyo3[49]={16,17,19,21,23,25,28,31,34,37,41,45,50,
		55,60,66,73,80,88,97,107,118,130,143,157,173,
		190,209,230,253,279,307,337,371,408,449,494,544,598,
		658,724,796,876,963,1060,1166,1282,1411,1552},
	hyo4[16]={-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};

void	error(x)
int	x;
{
	if( x == 0 )
		puts("�T���v�����O�f�[�^���r���ŏI����Ă��܂��B");
	else
		if( x == 2 )
			puts("�f�[�^�T�C�Y���U�S�L���o�C�g���z���Ă��܂��܂��B");
		else
			puts("�`�F�b�N�T���G���[�ł��B");
	if( ptr != 0 ){
		fwrite(text, 1, ptr, fp2);
		puts("�s���S�ȏo�̓t�@�C���𐶐����܂����B");
	} else {
		fclose(fp2);
		unlink(fn2);
		puts("�o�̓t�@�C���͐�������܂���ł����B");
	}
	exit(-1);
}

int	get_data()
{
	int	b,c=0,d;

	if( flag == 0 ){
		if( z == 0 ){
			if( (a=fgetc(fp1)) == EOF ) error(0);
			b=a & 0x0f;
			z++;
		} else {
			b=(a>>4) & 0x0f;
			z--;
		}
		d=hyo3[x];
		if( b & 4 )
			c+=d;
		d>>=1;
		if( b & 2 )
			c+=d;
		d>>=1;
		if( b & 1 )
			c+=d;
		d>>=1;
		c+=d;
		if( b & 8 )
			c*=-1;
		y+=c;
		x+=hyo4[b];
		if( x < 0 )
			x=0;
		else
			if( x > 48 )
				x=48;
		if( (short)y > 0 )
			data=0^p;
		else
			data=-1^p;
	} else
		flag--;
	return(data);
}

int	get_bit()
{
	int	x=0,c=0;

	while( get_data() == 0 );
	while( get_data() )
		c++;
	while( get_data() == 0 )
		c++;
	while( get_data() );
	if( c < r ){
		x++;
		while( get_data() == 0 );
		while( get_data() );
		while( get_data() == 0 );
		while( get_data() );
	}
	return(x);
}

int	get_byte()
{
	int i,x=0;

	while( get_bit() );
	for( i=0; i<8; i++ ){
		x>>=1;
		if( get_bit() )
			x|=0x80;
	}
	return(x);
}

void	cload()
{
	int c,i,start,sum,x,y,n;

	do {
		c=0;
		while( get_data() == 0 );
		while( get_data() )
			c++;
		while( get_data() == 0 )
			c++;
		flag++;
	} while( c < r );
	if( (c=get_byte()) != ':' )
		error(1);

	sum=x=get_byte();
	sum+=y=get_byte();
	sum+=get_byte();
	if( (sum % 0x100) != 0 )
		error(1);
	start=(x<<8)+y;
	printf("���[�h���ł��B�i�X�^�[�g�A�h���X : %.4X�j\n",start);

	while( 1 ){
		if( (c=get_byte()) != ':' )
			error(1);
		if( (sum=n=get_byte()) == 0 )
			break;
		if( n+ptr > 0x10000 )
			error(2);
		do {
			sum+=text[ptr++]=get_byte();
		} while( --n );
		sum+=get_byte();
		if( (sum % 0x100) != 0 )
			error(1);
	}
}

void	main(argc,argv)
int	argc;
char	*argv[];
{
	int	i,num=0;

	puts("P8cload Ver.1.10 Copyright 1994 K.Kuromusha");

	for( i=1; i<argc; i++ ){
		if( argv[i][0] == '-' || argv[i][0] == '/' ){
			switch( tolower(argv[i][1]) ){
				case 'p':
					p=-1;
					break;
				case 'r':
					switch( tolower(argv[i][2]) ){
						case '0':
							r=3;
							break;
						case '1':
							r=4;
							break;
						case '2':
							r=8;
							break;
						default:
							i=0;
					}
					break;
				default:
					i=0;
			}
			if( i == 0 )
				break;
		} else {
			num++;
			if( num == 1 )
				fn1=argv[i];
			else
				if( num == 2 )
					fn2=argv[i];
				else
					break;
		}
	}

	if( num != 2 ){
		puts("�g���� : P8cload [-p -r#] <filename1> <filename2>");
		puts("          -p       : �ʑ����]");
		puts("          -r0      : �T���v�����O���g��  7.8kHz�i�f�t�H���g�j");
		puts("          -r1      : �T���v�����O���g�� 10.4kHz");
		puts("          -r2      : �T���v�����O���g�� 15.6kHz");
		puts("         filename1 : �ϊ�ADPCM�t�@�C����");
		puts("         filename2 : �o�̓t�@�C����");
		if( i == 0 )
			puts("�����ȃI�v�V�������w�肵�܂����B");
		exit(-1);
	}

	if( (fp1=fopen(fn1,"rb")) == NULL ){
		puts("�ϊ�ADPCM�t�@�C���̃I�[�v���Ɏ��s���܂����B");
		exit(-1);
	}
	if( (fp2=fopen(fn2,"wb")) == NULL ){
		puts("�o�̓t�@�C���̃I�[�v���Ɏ��s���܂����B");
		exit(-1);
	}

	cload();

	fwrite(text, 1, ptr, fp2);
	fcloseall();
}
