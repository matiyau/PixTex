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
	char *filename, opt=0, wr_opt=0, exit=0;
	if (argc==1)
	{
		filename="sample.bmp";
	}
	else if ((argc>6 && (*argv[2]==50 || *argv[2]==0)) || (argc>4 && (*argv[2]!=50 && *argv[2]!=0)))
	{
		printf("Too Many Arguments. Arguments Were Not Accepted. Switched To Defaults \"sample.bmp\"\n\n");
		exit=0;
		opt=0;
		wr_opt=0;
		filename="sample.bmp";		
		goto start;
	}
	if (argc>=2)
	{
		if (*argv[1]==0)
		{
			filename="sample.bmp";
		}
		else
		{
			filename=argv[1];
		}		
	}
	if (argc>=3)
	{
		if ((*argv[2]==0)||(*argv[2]>=48 && *argv[2]<=51))
		{
			opt=*argv[2];
		}
		else
		{
			printf("Second Argument Is Invalid. Arguments Were Not Accepted. Switched To Defaults.\n\n");
			exit=0;
			opt=0;
			wr_opt=0;
			filename="sample.bmp";
			goto start;
		}
	}
	
	if ((argc==6 || argc==5) && opt==0)
	{
		opt=50;
	}
		
	if ((opt==49 || opt==51 || opt==0) && argc==4)
	{
		if (*argv[3]==48)
		{
			if (opt!=0)
			{
				exit=*argv[3];
			}
			else
			{
				printf("Second Argument Is Invalid. Arguments Were Not Accepted. Switched To Defaults.\n\n");
				opt=0;
				wr_opt=0;
				exit=0;
				filename="sample.bmp";
				goto start;
			}
		}
		else
		{
			if (opt==0)
			{
				printf("Third And Fourth Arguments Are");
			}
			else
			{
				printf("Fourth Argument Is ");
			}
			printf(" Invalid. Arguments Were Not Accepted. Switched To Defaults.\n\n");
			opt=0;
			wr_opt=0;
			exit=0;
			filename="sample.bmp";
			goto start;
		}
	}
	
	if (opt==50)
	{
		if (argc==4 || argc==5 || argc==6)
		{
			if ((*argv[2]==0)||(*argv[2]>=48 && *argv[2]<=51))
			{
				wr_opt=*argv[3];
			}
			else
			{
				printf("Third Argument Is Invalid. Arguments Were Not Accepted. Switched To Defaults.\n\n");
				opt=0;
				wr_opt=0;
				exit=0;
				filename="sample.bmp";
				goto start;
			}
		}
		if (argc==5 || argc==6)
		{
			if (*argv[4]==48 || *argv[4]==0)
			{
				exit=*argv[4];
			}
			else
			{
				if (argc==6)
				{
					argc=0;
				}
				printf("Fifth Argument Is Invalid. Arguments Were Not Accepted. Switched To Defaults.\n\n");
				opt=0;
				wr_opt=0;
				exit=0;
				filename="sample.bmp";
				goto start;
			}
		}
	}		
	
	start:
	fflush(stdin);
	if (opt!=48)
	{
		printf("The Current Image File Is \"%s\"\n", filename);
	}
	if (!opt)
	{
		printf("Enter 1 To Read The Message.\nEnter 2 To Write A Message.\nEnter 3 To Reset The Bitmap.\nEnter 0 To Exit.\n");
		scanf("%c", &opt);
		getchar();
	}
	
	if (opt==48)
	{
		return 0;
	}
	steg = fopen(filename, "rb+");
	if (steg==NULL)
	{
		printf("The Data File Does Not Exist.\n\n");
		exit=0;
		opt=0;
		wr_opt=0;
		goto start;
	}
	intrpt_header(steg);
	fseek(steg, 54, SEEK_SET);	
	if (opt==49)
	{
		opt=(0|exit);
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
		opt=(0|exit);
		char ch;
		int i,j;
		if (!wr_opt)
		{		
			printf("\n--Write Mode--\nEnter 0 To OverWrite Message\nEnter 1 To Add Text To The Existing Message\n");
			scanf("%c", &wr_opt);
			getchar();
		}
		if (wr_opt==48)
		{
			wr_opt=0;
			write_pix(2);		
		}
		else if (wr_opt==49)
		{
			wr_opt=0;
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
						fseek(steg, -sizeof(pixel), SEEK_CUR);
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
		if (argc==6)
		{			
			argc=0;
			do
			{
				fseek(steg, -sizeof(pixel), SEEK_CUR);
				write_pix(*(argv[5]+i));
				i+=1;
				if (i==bitmap.width)
				{
					fseek(steg, bitmap.padding, SEEK_CUR);
					i=0;
				}
				fseek(steg, sizeof(pixel), SEEK_CUR);
			}
			while(*(argv[5]+i)!=0);
			fseek(steg, -sizeof(pixel), SEEK_CUR);
			write_pix(10);
			fseek(steg, sizeof(pixel), SEEK_CUR);
		}
		else
		{
			printf("Please Enter Your Message :\n");
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
		}
		printf("\n--Message Recorded--\n\n");
		fclose(steg);		
	}
	else if (opt==51)
	{
		opt=(0|exit);
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
		opt=0;
		wr_opt=0;
		exit=0;
		printf("Invalid Input !\n\n");	
	}	
	goto start;
}
