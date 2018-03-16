#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define BITMAPFILEHEADERLENGTH 14 // The BMP file header length is 14
#define BMPFILETYPE 19778         // The ASCII code for BM

/* Test the file is bmp or not */
int isBmpFile( FILE* );
/* To get the OffSet of header to data part */
int getHeaderPartLength( FILE* );
/* To get the width of the bmp file */
int getBmpWidth( FILE* );
/* To get the height of the bmp file */
int getBmpHeight( FILE* );
/* Read bmp data and save to file */
int readBmpData( FILE* fpbmp, int offset, int width, int height );

int Decrypt( void );

int main( void )
{
	FILE* fpbmp;
	FILE* fpout;
	FILE* fptemp;
	
	int   offset = 0;
	int   width  = 0;
	int   height = 0;
	
	int choose = 0;
	
	fpbmp = fopen("exbmp.bmp","r");
	
	if( fpbmp == NULL )
	{
		printf("Error : Open file failed, please check the file is existence ot not!\n");
		return 1;
	}
	else
		printf("Open File ... OK!\n");
	
	if( isBmpFile( fpbmp ) != 0 )
	{
		printf("Error : the file is not a bmp file\n");
		return 1;
	}
	else
		printf("Check bmp file ... OK!\n");
	
	offset = getHeaderPartLength( fpbmp );
	width  = getBmpWidth( fpbmp );
	height = getBmpHeight( fpbmp );
	
	printf("Bmp Width  : %d pix\n", width);
	printf("Bmp Height : %d pix\n", height);
	
	if(readBmpData( fpbmp, offset, width, height ) != 0)
	{
		printf("Error : Read bmp data failed!\n");
		return 1;
	}
	else
		printf("Read bmp data ... OK!\n");
	
	printf( "Please choose what you want to do:\n" );
	printf( "1.Encrypt\n" );
	printf( "2.Decrypt\n" );
	
	scanf("%d",&choose);
	
	if( choose == 1 )
	{
	
		if(Encrypt()!=0)
		{
			printf("Encrypt failed!\n");
			return 1;
		}
		else
			printf("Encrypt ... OK!\n");
		
		fpout = fopen( "output.bmp","wb+" );
		
		fseek(fpbmp, 0L, SEEK_SET);
		fseek(fpout, 0L, SEEK_SET);
	
		fptemp=malloc(offset);
	    fread(fptemp, 1, offset, fpbmp);
		fwrite(fptemp,1,offset,fpout);
		
		if(outputBmpData( fpout, offset, width, height )!=0)
		{
			printf("Output file failed\n");
			return 1;
		}
		else
			printf("Output encrypted file ... OK!\n");
		
		fclose(fpout);
		
	}
	else if( choose == 2 )
	{
		Decrypt();
	}
	else
		;
	fclose( fpbmp );
	
	return 0;
}

int isBmpFile( FILE* fp )
{
	unsigned bmpfiletype = 0;
	
	fseek(fp, 0L, SEEK_SET);	// Reset file seek
	
	// Byte #0-1 save the typecode of the bmp file
	fread(&bmpfiletype, sizeof(char), 2, fp);

	if (BMPFILETYPE != bmpfiletype)
		return 1;	            // The file is not bmp file
	else
		return 0;
}

int getHeaderPartLength(FILE* fpbmp)
{
	int offset = 0;
	
	fseek(fpbmp, 10L, SEEK_SET);
    fread(&offset, sizeof(char), 4, fpbmp);
	  
    return offset;
}

int getBmpWidth(FILE* fpbmp)
{
	int width = 0;
	
    fseek(fpbmp, 18L, SEEK_SET);
    fread(&width, sizeof(char), 4, fpbmp);
	
	return width;
}

int getBmpHeight( FILE* fpbmp )
{
	int height = 0;
	
	fseek(fpbmp, 22L, SEEK_SET);
    fread(&height, sizeof(char), 4, fpbmp);
	
	return height;
}

int readBmpData( FILE* fpbmp, int offset , int width, int height )
{
	int i = 0, j = 0;
	unsigned char * pix = NULL;
	
	//one pix have 3 byte data( R G B )
	pix = malloc( 3 * sizeof( unsigned char ) );
	
	FILE* fp_rgb_r;
	FILE* fp_rgb_g;
	FILE* fp_rgb_b;
	
	fp_rgb_r = fopen( "DataRGB_R.txt","w" );
	fp_rgb_g = fopen( "DataRGB_G.txt","w" );
	fp_rgb_b = fopen( "DataRGB_B.txt","w" );

	fseek(fpbmp, offset, SEEK_SET);	// Jump to data part

	
	for( i = 0; i < height; i++ )
	{
		for( j = 0; j < width; j++ )
		{
			fread(pix, 3, 1, fpbmp);
			fprintf(fp_rgb_r,"%d\n",pix[0]);
			fprintf(fp_rgb_g,"%d\n",pix[1]);
			fprintf(fp_rgb_b,"%d\n",pix[2]);
		}
	}
	fclose(fp_rgb_r);
	fclose(fp_rgb_g);
	fclose(fp_rgb_b);
	
	return 0;
}

int outputBmpData( FILE* fpbmp, int offset, int width, int height )
{
	int i = 0, j = 0;
	unsigned char pix[3];
	
	FILE* fp_rgb_r;
	FILE* fp_rgb_g;
	FILE* fp_rgb_b;
	
	fp_rgb_r = fopen( "DataRGB_Ro.txt","r" );
	fp_rgb_g = fopen( "DataRGB_G.txt","r" );
	fp_rgb_b = fopen( "DataRGB_B.txt","r" );

	fseek(fpbmp, offset, SEEK_SET);	// Jump to data part
	
	for( i = 0; i < height; i++ )
	{
		for( j = 0; j < width; j++ )
		{	
			fscanf(fp_rgb_r,"%d\n",&pix[0]);
			fscanf(fp_rgb_g,"%d\n",&pix[1]);
			fscanf(fp_rgb_b,"%d\n",&pix[2]);
			fwrite(pix, 3, 1, fpbmp);
		}
	}
	fclose(fp_rgb_r);
	fclose(fp_rgb_g);
	fclose(fp_rgb_b);
	
	return 0;
}

int Encrypt( void )
{
	char str[100];
	FILE* fp_read_r;
	FILE* fp_out_r;
	char * cplen = str;
	int strlen = 0;
	int i = 0;
	int temp;
	
	printf("Please input the string:");
	scanf("%s",str);
	
	//calculate the string length
	while( *cplen++ != '\0' )
	{
		strlen++;
	}
	
	printf("The string length : %d\n",strlen);
	fp_read_r = fopen( "DataRGB_R.txt",  "r" );
	fp_out_r  = fopen( "DataRGB_Ro.txt", "w" );
	
	for( i = 0; i < strlen; i++ )
	{
		fscanf( fp_read_r,"%d\n",&temp );
		fprintf(fp_out_r,"%d\n",*(str+i));
	}
	
	fscanf( fp_read_r,"%d\n",&temp );
	fprintf(fp_out_r,"%d\n",0);
	
	while( !feof( fp_read_r ) )
	{
		fscanf( fp_read_r,"%d\n",&temp );
		fprintf( fp_out_r,"%d\n",temp );
	}
	
	fclose(fp_read_r);
	fclose(fp_out_r);
	
	return 0;
}

int Decrypt( void )
{
	int temp;
	FILE* fp = fopen( "DataRGB_R.txt", "r" );
	
	while( !feof(fp) )
	{
		fscanf(fp,"%d\n",&temp);
		if( temp != 0 )
		{
			printf("%c",temp);
		}
		else
		{	
			fclose(fp);
			break;
		}
	}
	printf("\nPress anykey to continue");
	getch();
	return 0;
}
