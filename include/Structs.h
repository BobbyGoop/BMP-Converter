#pragma once
#pragma pack(push,1)
struct BITMAPFILEHEADER
{
	unsigned short Type; // ‘BM’ 0x4D42
	unsigned long Size; // Размер файла в байтах, BitCount*Height*Width+ OffsetBits
	unsigned short Reserved1; // Зарезервирован; должен быть нуль
	unsigned short Reserved2; // Зарезервирован; должен быть нуль
	unsigned long OffsetBits; // Смещение данных от начала файла в байтах

	// = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
};
#pragma pack(pop)

#pragma pack(push,1)
struct BITMAPINFOHEADER
{
	unsigned long Size; // Число байтов необходимое для структуры = 40
	unsigned long Width; // Ширина точечного рисунка в пикселях
	unsigned long Height; // Высота точечного рисунка в пикселях
	unsigned short Planes; // Число плоскостей целевого устройства = 1
	unsigned short BitCount; // Глубина цвета, число бит на точку = 0,1,4,8,16,24,32
	unsigned long Compression; // Тип сжатия = 0 для несжатого изображения
	unsigned long SizeImage; // Размер изображения в байтах BitCount*Height*Width
	unsigned long XPelsPerMeter; // Разрешающая способность по горизонтали
	unsigned long YPelsPerMeter; // Разрешающая способность по вертикали
	unsigned long ColorUsed; // Число индексов используемых цветов. Если все цвета = 0
	unsigned long ColorImportant; // Число необходимых цветов = 0
};
#pragma pack(pop)

#pragma pack(push,1)
struct RGBTRIPLE
{
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
};
#pragma pack(pop)

#pragma pack(push,1)
struct RGBQUAD
{
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Reserved;
};
#pragma pack(pop)