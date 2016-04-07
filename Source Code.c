#include<stdio.h>

typedef struct pix
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
}pixel;

struct bmp
{
	int height;
	int width
}bitmap;

char conv_pix2char(pixel ip)
{
		char op = (((ip.B)%5)*25 + ((ip.G)%5)*5 + ((ip.R)%5));
		if (op==0)
		{
			op=125;
		}
		return op;
}

pixel conv_char2pix(char ip, pixel op)
{
	if (op.B==255)
	{
		op.B=250;
	}
	if (op.G==255)
	{
		op.G=250;
	}
	if (op.R==255)
	{
		op.R=250;
	}
	op.B=((op.B)/5)*5 + ((ip/25)%5);
	op.G=((op.G)/5)*5 + ((ip/5)%5);
	op.R=((op.R)/5)*5 + (ip%5);
	return op; 
}

void read_data(FILE *img)
{
	fseek(img, 54, SEEK_SET);
	printf("\n--Read Mode--\n");
	pixel map;
	int i, j, padding=0;
	char ch=0;
	if ((bitmap.width)%4!=0)
	{
		padding=4-((3*(bitmap.width))%4);
	}
	fread(&map, sizeof(pixel), 1, img);
	if (conv_pix2char(map)!=2)
	{
		printf("No Existing Data Found !\n\n");
		return;
	}
	for(i=0;i<bitmap.height;i++)
	{
		for(j=0;j<bitmap.width;j++)
		{
			fread(&map, sizeof(pixel), 1, img);
			ch=conv_pix2char(map);
			if (ch==10)
			{
				printf("\n--End Of Message--\n\n");
				fclose(img);
				return;
			}
			printf("%c", ch);
		}
		fseek(img, padding, SEEK_CUR);
	}	
}

void write_data(FILE *img)
{
	fseek(img, 54, SEEK_SET);
	printf("\n--Write Mode--\nEnter 0 To OverWrite Message\nEnter 1 To Add Text To The Existing Message\n");
	pixel map;
	int i, j, padding=0;
	char wr_opt, ch=0;
	if ((bitmap.width)%4!=0)
	{
		padding=4-((3*(bitmap.width))%4);
	}
	scanf("%c", &wr_opt);
	getchar();
	if (wr_opt==48)
	{
		fread(&map, sizeof(pixel), 1, img);
		fseek(img, -sizeof(pixel), SEEK_CUR);
		map=conv_char2pix(2, map);
		fwrite(&map, sizeof(pixel), 1, img);		
	}
	else if (wr_opt==49)
	{
		fread(&map, sizeof(pixel), 1, img);
		if (conv_pix2char(map)==2)
		{
			for(i=0;i<bitmap.height;i++)
			{
				for(j=0;j<bitmap.width;j++)
				{
					fread(&map, sizeof(pixel), 1, img);
					if (conv_pix2char(map)==10)
					{
						fseek(img, -sizeof(pixel), SEEK_CUR);
						break;
					}
				}
				if (j<bitmap.width)
				{
					break;
				}
				fseek(img, padding, SEEK_CUR);					
			}
		}
		else
		{
			printf("No Existing Data Found. Please Start From Scratch.\n");
			fseek(img, -sizeof(pixel), SEEK_CUR);
			map=conv_char2pix(2, map);
			fwrite(&map, sizeof(pixel), 1, img);
		}	
	}
	else
	{
		printf("Invalid Input !\n\n");
		return;
	}
	fseek(img, sizeof(pixel), SEEK_CUR);
	i=0;	
	do
	{
		fseek(img, -sizeof(pixel), SEEK_CUR);
		ch=getchar();
		fread(&map, sizeof(pixel), 1, img);
		map=conv_char2pix(ch, map);
		fseek(img, -sizeof(pixel), SEEK_CUR);
		fwrite(&map, sizeof(pixel), 1, img);
		i+=1;
		if (i==bitmap.width)
		{
			fseek(img, padding, SEEK_CUR);
			i=0;
		}
		fseek(img, sizeof(pixel), SEEK_CUR);
	}
	while(ch!=10);
	printf("\n--Message Recorded--\n\n");
	fclose(img);
	return;
}

void reset_data(FILE *img)
{
	fseek(img, 54, SEEK_SET);
	printf("\n--Reset Mode--\nPlease Wait");
	int i,j, padding;
	pixel map;
	if ((bitmap.width)%4!=0)
	{
		padding=4-((3*(bitmap.width))%4);
	}
	fseek(img, sizeof(pixel), SEEK_CUR);
	for(i=0;i<bitmap.height;i++)
	{
		for(j=0;j<bitmap.width;j++)
		{
			fseek(img, -sizeof(pixel), SEEK_CUR);
			fread(&map, sizeof(pixel), 1, img);
			map=conv_char2pix(3, map);
			fseek(img, -sizeof(pixel), SEEK_CUR);
			fwrite(&map, sizeof(pixel), 1, img);
			fseek(img, sizeof(pixel), SEEK_CUR);	
		}
		fseek(img, padding, SEEK_CUR);
		if (i%8==0)
		{
			printf(".");
		}
	}
	fclose(img);
	printf("\n--Bitmap Reset Successful--\n\n");
	return;
}

void intrpt_header(FILE *img)
{
	unsigned char head[54];
	fread(head, sizeof(unsigned char), 54, img);
	bitmap.height=(int)head[18] + (int)head[19]*256 + (int)head[20]*256*256 + (int)head[21]*256*256*256;
	bitmap.width=(int)head[22] + (int)head[23]*256 + (int)head[24]*256*256 + (int)head[25]*256*256*256;
	return;
}

int main()
{
	char opt;
	start:
	printf("Enter 1 To Read The Message.\nEnter 2 To Write A Message.\nEnter 3 To Reset The Bitmap.\nEnter 0 To Exit.\n");
	scanf("%c", &opt);
	getchar();
	if (opt==48)
	{
		return 0;
	}
	FILE *steg = fopen("sample.bmp", "rb+");
	if (steg==NULL)
	{
		printf("The Data File Does Not Exist.\n");
		return 1;
	}
	intrpt_header(steg);	
	if (opt==49)
	{
		read_data(steg);
	}
	else if (opt==50)
	{
		write_data(steg);
	}
	else if (opt==51)
	{
		reset_data(steg);
	}
	else 
	{
		printf("Invalid Input !\n\n");	
	}	
	goto start;
}
