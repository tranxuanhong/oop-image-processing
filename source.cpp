#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

#define MAX					100
#define	MAX_ARR				10000

using namespace std;

string filepathIn = "D:\\new-ascii.pgm"; // Thu muc mac dinh file input
string filepathOutNegative = "D:\\Negative.pgm";
string filepathOutLogTrans = "D:\\LogTrans.pgm";
string filepathOutHistoEqua = "D:\\Historam.pgm";
string filepathOutSmooth = "D:\\SmoothLinear.pgm";
string filepathOutLapa = "D:\\Lapacian.pgm";

bool flag = false;

class Filter;
class Image;

class Image {
public:
	int width;
	int height;
	int graylevels;
	int **pixels;

	void ReadImage(string filepath);
	void WriteFile(string filepath, Filter filter);
};

class Filter {
public:
	int width;
	int height;
	int graylevels;
	int **matrix;

	void NegativeImage(Image image);
	void LogTransformation(Image image);
	void HistogramEqua(Image image);
	void SmoothLinear(Image image);
	void Lapacian(Image image);
};


void Image::ReadImage(string filepath) {
	ifstream input;
	stringstream ss;
	string inputLine = "";

	input.open(filepath.c_str(), ios::in);
	if (input.fail()) {
		flag == true;
		cout << "Input file is fail !\nPlease redirect the file path\n";
	}
	//First line : version
	getline(input, inputLine);
	if (inputLine.compare("P2") != 0) {
		flag == true;
		cerr << "Version Error !" << endl;
	}

	else cout << "Version : " << inputLine << endl;

	//Second line : comment
	getline(input, inputLine);
	char first = inputLine[0];
	if (first == '#')
		cout << "Comment : " << inputLine << endl;
	else
		cout << "Please add the comment into the file first !" << endl;

	//Continue with a stringstream 
	ss << input.rdbuf();
	//Third line : size

	ss >> this->width >> this->height >> this->graylevels;
	cout << "Width : " << this->width << "; Height : " << this->height << "; Gray levels : " << this->graylevels << endl;

	this->pixels = new int*[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) {
		pixels[i] = new int[MAX_ARR];
	}

	for (int row = 0; row < this->height; ++row) {
		for (int col = 0; col < this->width; ++col) {
			ss >> pixels[row][col];
		}
	}
	input.close();
}

void Image::WriteFile(string filepath, Filter filter) {
	fstream output;
	output.open(filepath.c_str(), ios::out | ios::trunc);

	output << "P2" << endl;
	output << "# Image after processing" << endl;
	output << filter.width << " " << filter.height << endl;
	output << filter.graylevels << endl;
	for (int row = 0; row < filter.height; ++row) {
		for (int col = 0; col < filter.width; ++col) {
			output << filter.matrix[row][col] << " ";
		}
		output << endl;
	}
	output.close();
}

void Filter::NegativeImage(Image image) {
	this->matrix = new int*[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) {
		this->matrix[i] = new int[MAX_ARR];
	}
	this->width = image.width;
	this->height = image.height;
	this->graylevels = image.graylevels;

	for (int row = 0; row < this->height; ++row) {
		for (int col = 0; col < this->width; ++col) {
			this->matrix[row][col] = image.pixels[row][col] - 255;
		}
	}
}

void Filter::LogTransformation(Image image) {
	this->matrix = new int*[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) {
		this->matrix[i] = new int[MAX_ARR];
	}
	this->width = image.width;
	this->height = image.height;
	this->graylevels = image.graylevels;

	double c = 255 / log(256); // to scale the values between 0 and 255

	for (int row = 0; row < this->height; ++row) {
		for (int col = 0; col < this->width; ++col) {
			// f(x,y) = c*log[1+f(x,y)]
			this->matrix[row][col] = c*log(1 + image.pixels[row][col]);
		}
	}
}

void Filter::HistogramEqua(Image image) {
	this->matrix = new int*[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) {
		this->matrix[i] = new int[MAX_ARR];
	}
	this->width = image.width;
	this->height = image.height;
	this->graylevels = image.graylevels;

	int histogram[256];
	
	for (int i = 0; i < 256; i++) {
		histogram[i] = 0;
	}

	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			histogram[image.pixels[row][col]]++;
		}
	}

	double prob[256];
	int totalPixels = this->width*this->height;

	for (int i = 0; i < 256; i++) {
		prob[i] = (double)histogram[i] / totalPixels;
	}

	double cumHistogram[256];
	cumHistogram[0] = prob[0];
	for (int i = 1; i < 256; i++) {
		cumHistogram[i] = prob[i] + cumHistogram[i - 1];
	}

	int Scale[256];
	for (int i = 0; i < 256; i++) {
		Scale[i] = round((double)cumHistogram[i] * 255);
	}

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			this->matrix[row][col] = Scale[image.pixels[row][col]];
		}
	}
}

void Filter::SmoothLinear(Image image) {
	this->matrix = new int*[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) {
		this->matrix[i] = new int[MAX_ARR];
	}
	this->width = image.width;
	this->height = image.height;
	this->graylevels = image.graylevels;


	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			this->matrix[row][col] = 0;
		}
	}
	for (int row = 1; row < height - 1; row++) {
		for (int col = 1; col < width - 1; col++) {
			this->matrix[row][col] = (
				image.pixels[row - 1][col - 1] +
				image.pixels[row - 1][col] +
				image.pixels[row - 1][col + 1] +

				image.pixels[row][col - 1] +
				image.pixels[row][col] +
				image.pixels[row][col + 1] +

				image.pixels[row + 1][col - 1] +
				image.pixels[row + 1][col] +
				image.pixels[row + 1][col + 1]
				) / 9;
		}
	}
}

void Filter::Lapacian(Image image) {
	this->matrix = new int*[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) {
		this->matrix[i] = new int[MAX_ARR];
	}
	this->width = image.width;
	this->height = image.height;
	this->graylevels = image.graylevels;

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			this->matrix[row][col] = 0;
		}
	}
	for (int row = 1; row < height - 1; row++) {
		for (int col = 1; col < width - 1; col++) {
			this->matrix[row][col] = (
				image.pixels[row + 1][col] +
				image.pixels[row - 1][col] +
				image.pixels[row][col + 1] +
				image.pixels[row][col - 1] -
				4 * image.pixels[row][col]
				);
		}
	}
}

void Menu() {
	Image image;
	Filter filter;
	int choice;
	cout << "File is Loading... \n";
	image.ReadImage(filepathIn);
	if (flag == false) {
		do {
			cout << "1. Image Negative \n";
			cout << "2. Log Transformation \n";
			cout << "3. Histogram equalization \n";
			cout << "4. Smoothing Linear filter \n";
			cout << "5. Laplacian filter \n";
			cout << "Please enter your choice >> "; cin >> choice;
		} while (choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5);
		switch (choice) {
		case 1:
			filter.NegativeImage(image);
			image.WriteFile(filepathOutNegative, filter);
			break;
		case 2:
			filter.LogTransformation(image);
			image.WriteFile(filepathOutLogTrans, filter);

			break;
		case 3:
			filter.HistogramEqua(image);
			image.WriteFile(filepathOutHistoEqua, filter);
			break;
		case 4:
			filter.SmoothLinear(image);
			image.WriteFile(filepathOutSmooth, filter);
			break;
		case 5:
			filter.Lapacian(image);
			image.WriteFile(filepathOutLapa, filter);
			break;
		default:
			break;
		}
		cout << ">> The image is already processed ! <<\n";
		cout << ">> View the file to see the result  <<\n";
		cout << ">>--------------------------------------<<\n";
	}
	else
		cout << ">>--------------------------------------<<\n";
}

int main() {
	char choice;
	do {
		system("cls");
		Menu();
		do
		{
			cout << "Do you want to continue to process (Y/N) >> "; cin >> choice;
		} while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n');
	} while (choice == 'Y' || choice == 'y');
	system("pause");
	return 0;
}
