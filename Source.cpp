#define _CRT_SECURE_NO_WARNINGS
#include <locale>
#include "Image.h"
#include <iostream>

int main(void)
{
	setlocale(LC_ALL, "Russian");
	Image* source_1 = new Image((char*)"sunlight_32.bmp");
	
	/*Image * source_2 = new Image();
	source_2->loadimage((char*)"drone.bmp");*/

	Image* image_increased = new Image(0, source_1->getBitDepth(), 2560, 1440);
	Image* image_reduced = new Image(0, source_1->getBitDepth(), 240, 135);
	Image image_1, image_4, image_8;

	std::cout << " Увеличение размеров изображения ... ";
	(*image_reduced) /= *source_1;
	std::cout << "\t\tУспех!" << std::endl;
	
	std::cout << " Уменьшение размеров изображения ... ";
	(*image_increased) /= *source_1;
	std::cout << "\t\tУспех! " << std::endl;

	std::cout << " Понижение битовой глубины до 1 ...";
	image_1 = *source_1 / 1;
	std::cout << "\t\tУспех! " << std::endl;
	
	std::cout << " Понижение битовой глубины до 4 ...";
	image_4 = *source_1 / 4;
	std::cout << "\t\tУспех! " << std::endl;

	std::cout << " Понижение битовой глубины до 8  ...";
	image_8 = *source_1 / 8;
	std::cout << "\t\tУспех! " << std::endl;


	std::cout << " Сохранение файла с глубиной цвета 1 бит ... ";
	image_1.writeimage((char*)"sample_1.bmp");
	std::cout << "\tУспех! " << std::endl;

	std::cout << " Сохранение файла с глубиной цвета 4 бит ... ";
	image_4.writeimage((char*)"sample_4.bmp");
	std::cout << "\tУспех! " << std::endl;
	
	std::cout << " Сохранение файла с глубиной цвета 8 бит ... ";
	image_8.writeimage((char*)"sample_8.bmp");
	std::cout << "\tУспех! " << std::endl;

	std::cout << " Сохранение уменьшенного изображения ...";
	image_reduced->writeimage((char*)"sample_r.bmp");
	std::cout << "\tУспех! " << std::endl;
	
	std::cout << " Сохранение уменьшенного изображения ...";
	image_increased->writeimage((char*)"sample_i.bmp");
	std::cout << "\tУспех! " << std::endl;

	std::cout << " Конвертация прошла успешно. " << std::endl;
	system("pause");
	return 0;
}
