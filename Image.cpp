#define _CRT_SECURE_NO_WARNINGS
#include "Image.h"
#include <iostream>
#include <fstream>
using namespace std;

// Реализация конструктора без параметров
Image::Image()
{
	setEmptyImageParams(); // Заполняем параметры пустого изображения
}

/** 
* Реализация конструктора при загрузке из файла
* @param char* fileName: путь к файлу
*/
Image::Image(char* fileName)
{
	setEmptyImageParams(); // Заполняем параметры пустого изображения
	loadimage(fileName); // выполняем загрузку из файла с переданным именем
}

/**
* Реализация конструктора при при копировании данных
* из другого изображения
* @param const Image& im -  Изображение
*/
Image::Image(const Image& im)
{
	setEmptyImageParams();
	setHeaderAndAllocateMemory(im.BMInfoHeader.Width, im.BMInfoHeader.Height, im.BMInfoHeader.BitCount);
	copyDataFromImage(im);
}

/**
* Реализация конструктора при при копировании данных
* из другого изображения
* @param char Mode -  Режим
* @param unsigned short BCount -  Битовая глубина
* @param int Width -  Ширина
* @param int Height -  Высота
*/
Image::Image(char Mode, unsigned short BCount, int Width, int Height)
{
	setEmptyImageParams();
	setHeaderAndAllocateMemory(Width, Height, BCount);
	if (Palette) Mode = Palette[getNearestPaletteColorIndex(Mode)].Red;

	if (BCount <= 24)
	{
		for (int i = 0; i < (int)BMInfoHeader.Height; i++) 
		{
			for (int j = 0; j < (int)BMInfoHeader.Width; j++)
			{
				Rgbtriple[i * BMInfoHeader.Width + j].Red = Mode;
				Rgbtriple[i * BMInfoHeader.Width + j].Green = Mode;
				Rgbtriple[i * BMInfoHeader.Width + j].Blue = Mode;
			}
		}
	}
	else if (BCount == 32)
	{
		for (int i = 0; i < (int)BMInfoHeader.Height; i++)
		{
			for (int j = 0; j < (int)BMInfoHeader.Width; j++)
			{
				Rgbquad[i * BMInfoHeader.Width + j].Red = Mode;
				Rgbquad[i * BMInfoHeader.Width + j].Green = Mode;
				Rgbquad[i * BMInfoHeader.Width + j].Blue = Mode;
				Rgbquad[i * BMInfoHeader.Width + j].Reserved = 0;
			}
		}
	}
	else cout << "Ошибка: создание изображения битностью 16 не поддерживается"  << endl;
}

// Реализация деструктора
Image::~Image()
{
	if (Rgbtriple)
	{
		delete[] Rgbtriple;
		Rgbtriple = NULL;
	}
	if (Palette)
	{
		delete[] Palette;
		Palette = NULL;
	}
}

/**
* Метод загрузки изображения "вручную"
* @param char* fileName: путь к файлу
*/
int Image::loadimage(char* fileName)
{
	if (Rgbtriple || Rgbquad)
	{
		cout << "Ошибка: нельзя загружать данные в уже созданное изображение\n" << endl;
		return 0;
	}

	ifstream file;
	file.open(fileName, ios::binary);
		
	if (!file.is_open())
	{
		cout << " Ошибка: не удалось прочитать файл "<< fileName << endl;
		return 0;
	}

	BITMAPFILEHEADER BMFileHeader;
	BITMAPINFOHEADER BMInfoHeader;
	RGBQUAD* filePalette = NULL;

	file.read((char*)&BMFileHeader, sizeof(BITMAPFILEHEADER));
	file.read((char*)&BMInfoHeader, sizeof(BITMAPINFOHEADER));

	if (BMInfoHeader.BitCount != 24 && BMInfoHeader.BitCount != 32)
	{
		cout << " Ошибка: неподдерживаемая битность изображения: '%i'\n" << (int)BMInfoHeader.BitCount << endl;
		return 0;
	}

	setHeaderAndAllocateMemory(BMInfoHeader.Width, BMInfoHeader.Height, BMInfoHeader.BitCount);

	file.seekg(BMFileHeader.OffsetBits, ios_base::beg);
	const int additionalRowOffset = getAdditionalRowOffsetInFile(BMInfoHeader.Width, BMInfoHeader.BitCount);

	if (BMInfoHeader.BitCount == 24)
	{
		for (int i = BMInfoHeader.Height - 1; i >= 0; i--)
		{
			for (int k = 0; k < (int)BMInfoHeader.Width; k++) {
				RGBTRIPLE fileTriple;
				file.read((char*)&fileTriple, sizeof(RGBTRIPLE));
				Rgbtriple[i * BMInfoHeader.Width + k].Red = fileTriple.Red;
				Rgbtriple[i * BMInfoHeader.Width + k].Green = fileTriple.Green;
				Rgbtriple[i * BMInfoHeader.Width + k].Blue = fileTriple.Blue;
			}
			if (additionalRowOffset) file.seekg(additionalRowOffset, ios_base::cur);
		}
	}
	
	if (BMInfoHeader.BitCount == 32)
	{
		for (int i = BMInfoHeader.Height - 1; i >= 0; i--)
		{
			for (int k = 0; k < (int)BMInfoHeader.Width; k++) {
				RGBQUAD fileQuad;
				file.read((char*)&fileQuad, sizeof(RGBQUAD));
				Rgbquad[i * BMInfoHeader.Width + k].Red = fileQuad.Red;
				Rgbquad[i * BMInfoHeader.Width + k].Green = fileQuad.Green;
				Rgbquad[i * BMInfoHeader.Width + k].Blue = fileQuad.Blue;
				Rgbquad[i * BMInfoHeader.Width + k].Reserved = fileQuad.Reserved;
			}
			if (additionalRowOffset) file.seekg(additionalRowOffset, ios_base::cur);
		}
	}

	file.close();
	return 1;
}


// Заполняем необходимые поля информации об изображении
void Image::setHeaderAndAllocateMemory(int Width, int Height, int BitCount)
{
	BMInfoHeader.Width = Width;
	BMInfoHeader.Height = Height;
	BMInfoHeader.Planes = 1;
	BMInfoHeader.BitCount = BitCount;
	BMInfoHeader.SizeImage = getTotalImageSize(Width, Height, BitCount);

	if (BMInfoHeader.BitCount <= 8)
	{
		BMInfoHeader.ColorUsed = 1 << BMInfoHeader.BitCount; // степень двойки
		Palette = new RGBQUAD[BMInfoHeader.ColorUsed];
		for (int i = 0; i < (int)BMInfoHeader.ColorUsed; i++)
		{
			unsigned char color = (unsigned char)(255 * i / (BMInfoHeader.ColorUsed - 1));
			Palette[i].Red = color;
			Palette[i].Green = color;
			Palette[i].Blue = color;
			Palette[i].Reserved = 0;
		}
	}
	if (BMInfoHeader.SizeImage > 0) 
	{
		if (BMInfoHeader.BitCount <= 24) 
		{
			Rgbtriple = new RGBTRIPLE[BMInfoHeader.Height * BMInfoHeader.Width];
			Rgbquad = NULL;
		}
		if (BMInfoHeader.BitCount == 32) 
		{
			Rgbquad = new RGBQUAD[BMInfoHeader.Height * BMInfoHeader.Width];
			Rgbtriple = NULL;
		}
	
	};
}

// Бинарным поиском ищет ближайший цвет в палитре (так как она упорядочена по возрастанию оттенков серого)
// В данном случае невозможен проход циклами так как невозможно вычислить размер Palette
unsigned char Image :: getNearestPaletteColorIndex(unsigned char grayscaleColor) 
{
	int minIndex = 0;
	int maxIndex = BMInfoHeader.ColorUsed - 1;
	while (maxIndex >= minIndex)
	{
		int middleIndex = (minIndex + maxIndex) / 2;
		if (Palette[middleIndex].Red == grayscaleColor) return middleIndex;

		else if (Palette[middleIndex].Red < grayscaleColor) minIndex = middleIndex + 1;

		else maxIndex = middleIndex - 1;
	}

	if (minIndex == BMInfoHeader.ColorUsed) return (unsigned char)BMInfoHeader.ColorUsed - 1;
	if (minIndex == 0) return 0;
			
	int prevDelta = grayscaleColor - Palette[minIndex - 1].Red; 
	int nextDelta = Palette[minIndex].Red - grayscaleColor;
	return prevDelta < nextDelta ? minIndex - 1 : minIndex; 
}


// Вычисляет градацию серого для переданного цвета
unsigned char Image::getGrayscaleColor(unsigned char Red, unsigned char Green, unsigned char Blue) 
{
	int result = (int)(Red * 0.299 + Green * 0.597 + Blue * 0.114);
	if (result > 255) result = 255;
	return (unsigned char)result;
}

// Возвращает полный размер данных при записи в файл
int Image::getTotalImageSize(int Width, int Height, unsigned short BitCount) 
{
	return (getImageRowSize(BMInfoHeader.Width, BMInfoHeader.BitCount) + getAdditionalRowOffsetInFile(Width, BitCount)) * Height;
}

// Возвращает сколько байт требуется для записи строки изображения в файл
int Image::getImageRowSize(int Width, unsigned short BitCount) 
{
	return (Width * BitCount + 7) / 8;
}

// Считает сколько нужно дописывать в файл байт после записи строки изображения,
// чтобы получить размер строки кратным 4
int Image::getAdditionalRowOffsetInFile(int Width, unsigned short BitCount)  
{
	int remainder = getImageRowSize(Width, BitCount) % 4;
	return remainder ? 4 - remainder : 0;
}

// Выставление параметров, соответствующих пустому изображению
void Image::setEmptyImageParams()
{
	Rgbtriple = NULL;
	Rgbquad = NULL;
	Palette = NULL;
	BMInfoHeader.Size = 40;
	BMInfoHeader.Width = 0;
	BMInfoHeader.Height = 0;
	BMInfoHeader.Planes = 0;
	BMInfoHeader.BitCount = 0;
	BMInfoHeader.Compression = 0;
	BMInfoHeader.SizeImage = 0;
	BMInfoHeader.XPelsPerMeter = 0;
	BMInfoHeader.YPelsPerMeter = 0;
	BMInfoHeader.ColorUsed = 0;
	BMInfoHeader.ColorImportant = 0;
}

// Копируем данные изображения
void Image::copyDataFromImage(const Image& Inp)
{
	if (BMInfoHeader.Width == Inp.BMInfoHeader.Width &&
		BMInfoHeader.Height == Inp.BMInfoHeader.Height &&
		BMInfoHeader.BitCount == Inp.BMInfoHeader.BitCount)
		// Копирование данных через memcpy
		memcpy(Rgbtriple, Inp.Rgbtriple, BMInfoHeader.Height * BMInfoHeader.Width * sizeof(RGBTRIPLE));
	else cout << "Ошибка: в изображение уже проинициализированно другим разрешением и/или битностью" << endl;
}

/**
* Сохранение изображения в файл
* @param char* fileName: название выходного файла
*/
void Image::writeimage(char* fileName) 
{
	/*if (!Rgbtriple  || !Rgbquad)
	{
		cout << "Ошибка: в изображении нет данных для сохранения" << endl;
		return;
	}*/
	ofstream fileout;
	fileout.open(fileName, ios::binary);
	const int additionalRowOffset = getAdditionalRowOffsetInFile(BMInfoHeader.Width, BMInfoHeader.BitCount);

	BITMAPFILEHEADER BMFileHeader;
	BMFileHeader.Type = 0x4D42;
	BMFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BMFileHeader.Size = BMFileHeader.OffsetBits + getTotalImageSize(BMInfoHeader.Width, BMInfoHeader.Height, BMInfoHeader.BitCount);
	BMFileHeader.Reserved1 = 0;
	BMFileHeader.Reserved2 = 0;

	if (Palette)
	{
		const int paletteSize = sizeof(RGBQUAD) * BMInfoHeader.ColorUsed;
		BMFileHeader.Size += paletteSize;
		BMFileHeader.OffsetBits += paletteSize;
	}

	fileout.write((char*)&BMFileHeader, sizeof(BITMAPFILEHEADER));
	fileout.write((char*)&BMInfoHeader, sizeof(BITMAPINFOHEADER));

	if (BMInfoHeader.BitCount == 24)
	{
		for (int i = BMInfoHeader.Height - 1; i >= 0; i--)
		{
			for (int k = 0; k < BMInfoHeader.Width; k++) {

				fileout.write((char*)&Rgbtriple[i * BMInfoHeader.Width + k], sizeof(RGBTRIPLE));
			}
			if (additionalRowOffset)
			{
				const int Zero = 0;
				fileout.write((char*)&Zero, sizeof(RGBTRIPLE));
			}
		}
	}
	else if (BMInfoHeader.BitCount == 32)
	{
		for (int i = BMInfoHeader.Height - 1; i >= 0; i--)
		{
			for (int k = 0; k < BMInfoHeader.Width; k++) {

				fileout.write((char*)&Rgbquad[i * BMInfoHeader.Width + k], sizeof(RGBQUAD));
			}
			if (additionalRowOffset)
			{
				const int Zero = 0;
				fileout.write((char*)&Zero, sizeof(RGBQUAD));
			}
		}
	}
	else 
	{
		for (int c = 0; c < (int)BMInfoHeader.ColorUsed; c++) {
			fileout.write((char*)&Palette[c], sizeof(RGBQUAD));
		}

		const int startPaletteDataOffset = 8 - BMInfoHeader.BitCount;
		for (int i = BMInfoHeader.Height - 1; i >= 0; i--)
		{
			int currentPaletteDataOffset = startPaletteDataOffset;
			int leftBits = 8;
			int paletteByte = 0;
			for (int j = 0; j < (int)BMInfoHeader.Width; j++)
			{
				unsigned char paletteColorIndex = getNearestPaletteColorIndex(Rgbtriple[i * BMInfoHeader.Width + j].Red);
				paletteByte |= paletteColorIndex << currentPaletteDataOffset;
				currentPaletteDataOffset -= BMInfoHeader.BitCount;
				leftBits -= BMInfoHeader.BitCount;

				if (!leftBits)
				{
					fileout.write((char*)&paletteByte, 1);
					currentPaletteDataOffset = startPaletteDataOffset;
					leftBits = 8;
					paletteByte = 0;
				}
			}
			if (leftBits != 8) fileout.write((char*)&paletteByte, 1);
			if (additionalRowOffset)
			{
				const int Zero = 0;
				fileout.write((char*)&Zero, sizeof(RGBTRIPLE));
			}
		}
	}
	fileout.close();
}

// Перегрузка оператора =
Image& Image::operator = (const Image& Inp) 
{
	if (Rgbtriple) copyDataFromImage(Inp);
	else {
		setEmptyImageParams();
		setHeaderAndAllocateMemory(Inp.BMInfoHeader.Width, Inp.BMInfoHeader.Height, Inp.BMInfoHeader.BitCount);
		copyDataFromImage(Inp);
	}
	return *this;
}

// Функция преобразования данных переданного изображения в текущий формат
void Image::copyAndConvertDataFromImage(const Image& Inp)
{
	const bool isSourceWithPalette = Inp.Palette != NULL;
	for (int i = 0; i < (int)BMInfoHeader.Height; i++)
	{
		for (int j = 0; j < (int)BMInfoHeader.Width; j++)
		{
			unsigned char grayscale = NULL;
			if (isSourceWithPalette)
			{
				grayscale = Inp.BMInfoHeader.BitCount == 32 ? Inp.Rgbtriple[i * BMInfoHeader.Width + j].Red
					: Inp.Rgbquad[i * BMInfoHeader.Width + j].Red;
			}
			else
			{
				grayscale = getGrayscaleColor(Inp.Rgbquad[i * BMInfoHeader.Width + j].Red,
					Inp.Rgbquad[i * BMInfoHeader.Width + j].Green,
					Inp.Rgbquad[i * BMInfoHeader.Width + j].Blue);
			}

			grayscale = Palette[getNearestPaletteColorIndex(grayscale)].Red; 

			Rgbtriple[i * BMInfoHeader.Width + j].Red = grayscale;
			Rgbtriple[i * BMInfoHeader.Width + j].Green = grayscale;
			Rgbtriple[i * BMInfoHeader.Width + j].Blue = grayscale;
		}
	}
}

// Перегрузка оператора /
Image Image::operator / (short Depth) 
{
	if ((Depth == 8 || Depth == 4 || Depth == 1) && Depth <= BMInfoHeader.BitCount)
	{
		Image result(0, Depth, BMInfoHeader.Width, BMInfoHeader.Height);
		result.copyAndConvertDataFromImage(*this);
		return result;
	}
	else {
		cout << " Ошибка: неподдерживаемая битность" << endl;
		return Image(*this);
	}
}

// Перегрузка оператора /=
Image& Image::operator /= (const Image& Inp) 
{
	if (BMInfoHeader.BitCount != Inp.BMInfoHeader.BitCount)
	{
		cout<< " Ошибка: разная битность изображений, получение изображения с новым размером невозможно"<< endl;
		return *this;
	}

	float xRatio = (float)Inp.BMInfoHeader.Width / BMInfoHeader.Width;
	float yRatio = (float)Inp.BMInfoHeader.Height / BMInfoHeader.Height;

	if (BMInfoHeader.BitCount <= 24)
	{
		for (int i = 0; i < (int)BMInfoHeader.Height; i++)
		{
			for (int j = 0; j < (int)BMInfoHeader.Width; j++)
			{
				int sourceX = (int)(j * xRatio);
				int sourceY = (int)(i * yRatio);
				Rgbtriple[i * BMInfoHeader.Width + j] = Inp.Rgbtriple[sourceY * Inp.BMInfoHeader.Width + sourceX];
			}
		}
	}
	else if (BMInfoHeader.BitCount == 32)
	{
		for (int i = 0; i < (int)BMInfoHeader.Height; i++)
		{
			for (int j = 0; j < (int)BMInfoHeader.Width; j++)
			{
				int sourceX = (int)(j * xRatio);
				int sourceY = (int)(i * yRatio);
				Rgbquad[i * BMInfoHeader.Width + j] = Inp.Rgbquad[sourceY * Inp.BMInfoHeader.Width + sourceX];
			}
		}
	}
	
	return *this;
}
