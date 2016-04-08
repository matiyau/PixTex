#include<stdio.h>

FILE *steg;

struct pix
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
}pixel;

struct bmp
{
	int height;
	int width;
	int padding;
}bitmap;

char conv_pix2char()
{
		char op = (((pixel.B)%5)*25 + ((pixel.G)%5)*5 + ((pixel.R)%5));
		if (op==0)
		{
			op=125;
		}
		return op;
}

void conv_char2pix(char ip)
{
	if (pixel.B==255)
	{
		pixel.B=250;
	}
	if (pixel.G==255)
	{
		pixel.G=250;
	}
	if (pixel.R==255)
	{
		pixel.R=250;
	}
	pixel.B=((pixel.B)/5)*5 + ((ip/25)%5);
	pixel.G=((pixel.G)/5)*5 + ((ip/5)%5);
	pixel.R=((pixel.R)/5)*5 + (ip%5);
	return;
}

void intrpt_header(FILE *steg)
{
	unsigned char head[54];
	fread(head, sizeof(unsigned char), 54, steg);
	bitmap.height=(int)head[18] + (int)head[19]*256 + (int)head[20]*256*256 + (int)head[21]*256*256*256;
	bitmap.width=(int)head[22] + (int)head[23]*256 + (int)head[24]*256*256 + (int)head[25]*256*256*256;
	if ((bitmap.width)%4!=0)
	{
		bitmap.padding=4-((3*(bitmap.width))%4);
	}
	return;
}

char read_char()
{
	fread(&pixel, sizeof(pixel), 1, steg);
	return conv_pix2char();
}

void write_pix(char ch)
{
	fread(&pixel, sizeof(pixel), 1, steg);
	conv_char2pix(ch);
	fseek(steg, -sizeof(pixel), SEEK_CUR);
	fwrite(&pixel, sizeof(pixel), 1, steg);
	return;
}

int main(int argc, char *argv[])
{
	char *filename;
	if (argc==1)
	{
		filename="sample.bmp";
	}
	else if (argc==2)
	{
		filename=argv[1];
	}
	else
	{
		filename="sample.bmp";
		printf("Too Many Arguments. Switching To Default File Name \"sample.bmp\"");
	}	
	char opt;
	start:
	printf("The Current Image File Is \"%s\"\n", filename);
	printf("Enter 1 To Read The Message.\nEnter 2 To Write A Message.\nEnter 3 To Reset The Bitmap.\nEnter 4 To Change The Image File.\nEnter 0 To Exit.\n");
	scanf("%c", &opt);
	getchar();
	if (opt==48)
	{
		return 0;
	}
	else if (opt==52)
	{
		printf("\nPlease Key In The Name Of The New File : ");
		scanf("%[^\n]", filename);
		getchar();
		printf("\n");
		goto start;
	}
	steg = fopen(filename, "rb+");
	if (steg==NULL)
	{
		printf("The Data File Does Not Exist.\n\n");
		goto start;
	}
	intrpt_header(steg);
	fseek(steg, 54, SEEK_SET);	
	if (opt==49)
	{
		printf("\n--Read Mode--\n");
		int i, j;
		char ch;
		if (read_char()!=2)
		{
			printf("No Existing Data Found !\n\n");
			fclose(steg);
			goto start;
		}
		for(i=0;i<bitmap.height;i++)
		{
			for(j=0;j<bitmap.width;j++)
			{
				ch=read_char();
				if (ch==10)
				{
					printf("\n--End Of Message--\n\n");
					fclose(steg);
					goto start;
				}
				printf("%c", ch);
			}
			fseek(steg, bitmap.padding, SEEK_CUR);
		}
	}
	else if (opt==50)
	{
		char wr_opt,ch;
		int i,j;
		printf("\n--Write Mode--\nEnter 0 To OverWrite Message\nEnter 1 To Add Text To The Existing Message\n");
		scanf("%c", &wr_opt);
		getchar();
		if (wr_opt==48)
		{
			write_pix(2);		
		}
		else if (wr_opt==49)
		{
			if (read_char()==2)
			{
				for(i=0;i<bitmap.height;i++)
				{
					for(j=0;j<bitmap.width;j++)
					{
						if (read_char()==10)
						{
							break;
						}
					}
					if (j<bitmap.width)
					{
						break;
					}
					fseek(steg, bitmap.padding, SEEK_CUR);					
				}
			}
			else
			{
				printf("No Existing Data Found. Please Start From Scratch.\n");
				fseek(steg, -sizeof(pixel), SEEK_CUR);
				write_pix(2);
			}	
		}
		else
		{
			printf("Invalid Input !\n\n");
			fclose(steg);
			goto start;
		}
		fseek(steg, sizeof(pixel), SEEK_CUR);
		i=0;	
		do
		{
			fseek(steg, -sizeof(pixel), SEEK_CUR);
			ch=getchar();
			write_pix(ch);
			i+=1;
			if (i==bitmap.width)
			{
				fseek(steg, bitmap.padding, SEEK_CUR);
				i=0;
			}
			fseek(steg, sizeof(pixel), SEEK_CUR);
		}
		while(ch!=10);
		printf("\n--Message Recorded--\n\n");
		fclose(steg);		
	}
	else if (opt==51)
	{
		printf("\n--Reset Mode--\nPlease Wait");
		int i, j;
		fseek(steg, sizeof(pixel), SEEK_CUR);
		for(i=0;i<bitmap.height;i++)
		{
			for(j=0;j<bitmap.width;j++)
			{
				fseek(steg, -sizeof(pixel), SEEK_CUR);
				write_pix(3);
				fseek(steg, sizeof(pixel), SEEK_CUR);
			}
			fseek(steg, bitmap.padding, SEEK_CUR);
			if (i%8==0)
			{
				printf(".");
			}
		}
		fclose(steg);
		printf("\n--Bitmap Reset Successful--\n\n");
	}
	else 
	{
		printf("Invalid Input !\n\n");	
	}	
	goto start;
}
