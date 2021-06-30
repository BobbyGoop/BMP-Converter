#pragma once
#include <cstdio>
#include <fstream>
#include "Structs.h"
using namespace std;


class Image {
private:
	// �������������� ��������� �����������
	BITMAPINFOHEADER BMInfoHeader; 

	// ������ � ��������� �������� ���� RGBTRIPLE
	RGBTRIPLE* Rgbtriple;
	RGBQUAD* Rgbquad;

	// ������� ����������� (������������ ������, ���� ������� ����� ����� 1, 4 ��� 8)
	RGBQUAD* Palette; 
	
	void setEmptyImageParams();
	void setHeaderAndAllocateMemory(int Width, int Height, int BitCount); 
	void copyDataFromImage(const Image& Inp); 	
	void copyAndConvertDataFromImage(const Image& Inp); 
	int getAdditionalRowOffsetInFile(int Width, unsigned short BitCount);
	int getImageRowSize(int Width, unsigned short BitCount);
	int getTotalImageSize(int Width, int Height, unsigned short BitCount);
	unsigned char getGrayscaleColor(unsigned char Red, unsigned char Green, unsigned char Blue);
	unsigned char getNearestPaletteColorIndex(unsigned char grayscaleColor);

public:
	// ����������� �������� �����������
	Image(char Mode, unsigned short BCount, int Width, int Height);
		
	// ����������� ������� ����������� �� �����
	Image(char* fileName); 
		
	// ����������� ��� ����������, ������� ������ ��������� ��� �����������
	Image();
		
	// ����������� �����
	Image(const Image& i);
		
	// ����������
	~Image();
		
	// ����� �������� ����������� ����������� ������������, ���������� 0 � ������ ������
	int loadimage(char* fileName);
		
	// ����� ������ ����������� � ����
	void writeimage(char* fileName);

	int getBitDepth() { return BMInfoHeader.BitCount; };
		
	// ���������� ��������� "="
	Image& operator = (const Image& Inp);
		
	// ���������� ��������� /
	Image operator / (short Depth);
		
	// ���������� ��������� /=	
	Image& operator /= (const Image& Inp); 	
};
