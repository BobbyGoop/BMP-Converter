#pragma once
#include <cstdio>
#include <fstream>
#include "Structs.h"
using namespace std;


class Image {
private:
	// Информационный заголовок изображения
	BITMAPINFOHEADER BMInfoHeader; 

	// Массив с описанием пикселей типа RGBTRIPLE
	RGBTRIPLE* Rgbtriple;
	RGBQUAD* Rgbquad;

	// Палитра изображения (присутствует только, если глубина цвета равна 1, 4 или 8)
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
	// Конструктор создания изображения
	Image(char Mode, unsigned short BCount, int Width, int Height);
		
	// Конструктор объекта изображения из файла
	Image(char* fileName); 
		
	// Конструктор без параметров, создает пустой контейнер под изображение
	Image();
		
	// Конструктор копии
	Image(const Image& i);
		
	// Деструктор
	~Image();
		
	// Метод загрузки изображения аналогичный конструктору, возвращает 0 в случае ошибки
	int loadimage(char* fileName);
		
	// Метод записи изображения в файл
	void writeimage(char* fileName);

	int getBitDepth() { return BMInfoHeader.BitCount; };
		
	// Перегрузка оператора "="
	Image& operator = (const Image& Inp);
		
	// Перегрузка оператора /
	Image operator / (short Depth);
		
	// Перегрузка оператора /=	
	Image& operator /= (const Image& Inp); 	
};
