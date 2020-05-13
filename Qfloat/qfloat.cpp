﻿#include "qfloat.h"

bool Qfloat::getSign()
{
	return (data[0] & 1);
}

bool Qfloat::getExponent(int i)
{
	return((data[0] >> (i + 1)) & 1);
}

bool Qfloat::getSignificand(int i)
{
	if (i < 16)
		return ((data[0] >> (i + 16)) & 1);
	else
	{
		i += 16;
		int interval = i / 32;
		i = i % 32;
		return ((data[interval] >> i) & 1);
	}
}

void Qfloat::setSign(bool value)
{
	if (getSign() == value)
		return;
	data[0] ^= 1;
}

void Qfloat::setExponent(int i, bool value)
{
	if (getExponent(i) == value)
		return;
	data[0] = data[0] ^ (1 << (i + 1));
}

void Qfloat::setSignificand(int i, bool value)
{
	if (getSignificand(i) == value)
		return;
	if (i < 16)
	{
		data[0] = data[0] ^ (1 << (i + 16));
	}
	else
	{
		i += 16;
		int interval = i / 32;
		i = i % 32;
		data[interval] = data[interval] ^ (1 << i);
	}
}

Qfloat::Qfloat()
{
	//Khởi tạo tất cả các bit về 0
	memset(data, 0, sizeof(data));
}


Qfloat::Qfloat(string num) : Qfloat()
{
	//Chuẩn hóa 1 số để tránh người dùng nhập không chuẩn
	num = standardize(num);

	//Kiểm tra số vừa nhập bằng 0
	if (num == "0.0")
		return;
	//Kiểm tra số âm
	if (num[num.size() - 1] == '-')
	{
		num.resize(num.size() - 1);
		setSign(true);
	}
	
	//Tách phần nguyên và phần thập phân
	int i = 0;
	string intPart;
	string decPart;
	for (; num[i] != '.'; i++)
		decPart = decPart + num[i];
	i++;
	for (; i < num.size(); i++)
		intPart = intPart + num[i];
	
	//Chuyển phần nguyên và phần thập phân sang nhị phân số chấm tĩnh
	string staticPoint = decPartToBin(decPart);			
	int pointPosition = staticPoint.size();				
	staticPoint = staticPoint + intPartToBin(intPart);	

	//Tính significand
	int e = staticPoint.size() - 1;
	while (staticPoint[e] == '0')
		e--;
	for (int i = 0; i < significandSize; i++)
	{
		this->setSignificand(i, staticPoint[e] == '1');
		e--;
	}
	//Tính exponent
	e = biasingValue + (e - pointPosition);
	for (int i = 0; i < exponentSize; i++)
		this->setExponent(14-i, (e >> i) & 1);
}


Qfloat::Qfloat(bool* bitSeq) : Qfloat()
{
	int counter = 0;
	setSign(bitSeq[counter++]);
	for (int i = 0; i < exponentSize; i++)
	{
		setExponent(i, bitSeq[counter++]);
	}
	for (int i = 0; i < significandSize; i++)
	{
		this->setSignificand(i, bitSeq[counter++]);
	}
}

string Qfloat::getBits()
{
	string num;
	num = num + char('0' + this->getSign());
	for (int i = 0; i < exponentSize; i++)
		num = num + char('0' + this->getExponent(i));
	for (int i = 0; i < significandSize; i++)
		num = num + char('0' + this->getSignificand(i));
	return num;
}

/*
	Bạn bị đứng máy??!!
	Rất tiếc vì hiện tại tôi chưa tối ưu hóa code của mình
để tính toán các dãy bit có giá trị quá to hoặc quá nhỏ.
	Bạn có thể tham khảo phần cuối của file qfloat.h ở đó
tôi có để cho bạn 1 vài gợi ý mà tôi nghĩ nó sẽ làm chương
trình này chạy nhanh hơn (>.o)
	Cố lên!!!
*/
string Qfloat::getValue()
{
	//Kiểm tra số đặc biệt
	if (this->isZero())
		return string("0");
	if (this->isDenormalized())
		return string("Denormalized Number");
	if (this->isInf())
		return string("Inf");
	if (this->isNaN())
		return string("NaN");

	//Xác định dấu của số
	string ans;
	if (getSign() == true)
		ans = ans + '-';
	
	//Lấy giá trị ở hệ thập phân của exponent
	int e = 0;
	for (int i = 0; i < exponentSize; i++)
	{
		e = e | (this->getExponent(i) << (14 - i));
	}
	e = e - biasingValue;
	
	//Xác định bit 1 phải nhất của significand
	int lastOne = significandSize;
	while (this->getSignificand(lastOne - 1) == false)
		lastOne--;

	//Tính phần nguyên
	BigInt intPart;
	int pointPos = e;
	if (pointPos > significandSize)
		pointPos = significandSize;
	if (e >= 0)
	{
		BigInt Pow(1);
		for (int i = pointPos-1; i >= 0; i--)
		{
			if (this->getSignificand(i) == true)
				intPart = intPart + Pow;
			Pow = Pow * 2;
		}
		intPart = intPart + Pow;
	}
	//x << i = x * 2^i
	if (e > significandSize)
	{
		int time = e - significandSize;
		while (time)
		{
			int temp = 1;
			for (int i = 0; i < 28; i++)
			{
				if (time == 0)
					break;
				temp *= 2;
				time--;
			}
			intPart = intPart * temp;
		}
	}

	//Tính phần thập phân
	BigInt decPart, Pow(1);
	int realPowSize = 0;
	int realDecSize = 0;
	if (e < 0)
		pointPos = 0;
	else
		pointPos = e;
	for (int i = pointPos; i < lastOne; i++)
	{
		div2onDec(Pow, realPowSize);
		if (i >= 0 && this->getSignificand(i) == true)
			addDec(decPart, realDecSize, Pow, realPowSize);
	}
	//x >> i = x*2^(-i)
	if (e < 0)
	{
		//Giải quyết số 1 đầu tiên bên trái dấu phẩy
		BigInt Pow(5); //2^(-1)
		int realPowSize = 1;
		div2onDec(decPart, realDecSize);
		addDec(decPart, realDecSize, Pow, realPowSize);
		
		//Giải quyết cái số còn lại
		int time = -e - 1;
		for (int i = 0; i < time; i++)
		{
			div2onDec(decPart, realDecSize);
		}
	}

	//Ghép phần nguyên và phần thập phân lại với nhau
	ans = ans + intPart.getNum() + '.';
	for (int i = realDecSize; i > decPart.getSize(); i--)
		ans = ans + '0';
	ans = ans + decPart.getNum();
	return ans;
}


string Qfloat::intPartToBin(string str)
{
	BigInt num(str);
	string ans;
	if (num.isZero())
		return string("0");
	char rest;
	while (num.isZero() == false)
	{
		div2onInt(num,rest);
		ans = ans + rest;
	}
	return ans;
}


string Qfloat::decPartToBin(string str)
{
	BigInt num(str);
	int fixedSize = str.size();
	if (num.isZero())
		return string("0");
	string ans;
	char rest;
	//Bắt đầu đếm khi gặp bit 1 đầu tiên
	bool isCounting = false;
	//Biến đếm bit
	int counter = 0;
	while (num.isZero() == false)
	{
		mult2onDec(num, fixedSize, rest);
		//std::cout << num.getNum() << " " << num.getSize() << " " << rest << std::endl;
		//_getch();
		ans = ans + rest;
		//Gặp bit 1 đầu tiên --> bật chế độ đếm bit
		if (isCounting)
			counter++;
		if (rest == '1')
			isCounting = true;
		//Nếu kích thước phần cần lưu mà lớn hơn kích thước của significand thì dừng
		if (counter == significandSize)
			break;
	}
	std::reverse(ans.begin(), ans.end());
	return ans;
}


void Qfloat::div2onInt(BigInt& num, char& rest)
{
	rest = char('0' + (num % 2));
}


void Qfloat::mult2onDec(BigInt& num, int fixedSize, char& rest)
{
	num = num * 2;
	if (num.getSize() > fixedSize)
	{
		rest = '1';
		num.resize(fixedSize);
	}
	else
		rest = '0';
}

//Hàm cộng phần nguyên của 2 số
string Qfloat::addInt(string a, string b)
{
	string ans;
	if (a.size() < b.size())
		swap(a, b);
	int mem = 0;
	for (int i = 0; i < a.size(); i++)
	{
		if (i >= b.size())
			mem = mem + (a[i] - '0');
		else
			mem = mem + (a[i] - '0') + (b[i] - '0');
		ans = ans + char(mem % 10 + '0');
		mem = mem / 10;
	}
	if (mem > 0)
		ans = ans + '1';
	return ans;
}


void Qfloat::div2onDec(BigInt& num,int& realDecSize)
{
	num = num * 5;
	realDecSize++;
}

void Qfloat::addDec(BigInt& a,int& realASize, BigInt& b, int& realBSize)
{
	while (realASize < realBSize)
	{
		a = a * 10;
		realASize++;
	}
	while (realBSize > realASize)
	{
		b = b * 10;
		realBSize++;
	}
	a = a + b;
}


string Qfloat::standardize(string num)
{
	//Tìm vị trí của dấu chấm
	int pointPos = 0;
	while (pointPos < num.size() && num[pointPos] != '.')
		pointPos++;

	//Thêm vào phần thập phân = 0 nếu không có
	if (pointPos == num.size())
		num = num + ".0";

	//Xóa đi những số 0 vô nghĩa của phần thập phân
	while (num[num.size() - 1] == '0' && num[num.size() - 2] != '.')
		num.erase(num.end()-1);

	//Đảo chuỗi để tiện xử lý
	std::reverse(num.begin(), num.end());

	//Xóa đi những số 0 vô nghĩa của phần nguyên
	while (num[num.size() - 1] == '0' && num[num.size() - 2] != '.')
		num.erase(num.end()-1);

	//Nếu số được nhập là -0.0 thì đổi thành 0.0
	if (num == "0.0-")
		num = "0.0";
	return num;
}

Qfloat* Qfloat::decToBin(string num)
{
	Qfloat* newQfloat = new Qfloat(num);
	return newQfloat;
}



Qfloat* Qfloat::binToDec(string num)
{
	bool* bits = new bool[128];
	while (num.size() < 128)
		num = '0' + num;
	for (int i = 0; i < num.size(); i++)
		bits[i] = (num[i] == '1');
	Qfloat* newQfloat = new Qfloat(bits);
	return newQfloat;
}

int ScanQfloat(Qfloat* &num, std::istream& inp, std::ostream& outp)
{
	//Nhập 2 chỉ thị p1, p2 và chuỗi số
	int p1, p2;
	string str;
	inp >> p1 >> p2 >> str;

	//Kiểm tra chuỗi số vừa nhập có phải là con số hay không?
	if ((p1 == 10 && isDec(str) == false) ||
		(p1 ==  2 && isBin(str) == false))
	{
		printInputError(NOT_A_NUMBER, outp);
		return 0;
	}

	//Kiểm tra 2 chỉ thị có hợp lệ không
	if (!((p1 == 10 && p2 == 2) || (p1 == 2 && p2 == 10)))
	{
		printInputError(WRONG_BASE, outp);
	}
	
	//Chuyển chuỗi số vừa nhập được thành 1 đối tượng Qfloat
	if (p1 == 2)
		num = Qfloat::binToDec(str);
	else
		num = Qfloat::decToBin(str);
	
	//Trả về giá trị của p2
	return p2;
}

void PrintQfloat(Qfloat* num, int base, std::ostream& outp)
{
	if (base == 0)
		return;
	else if (base == 2)
		outp << num->getBits() << std::endl;
	else
		outp << num->getValue() << std::endl;
}

bool deleteSpace(string& str)
{
	int size = str.size();
	while (size > 0 && str[size - 1] == ' ')
		size--;
	if (size == 0)
		return false;
	str.resize(size);
	std::reverse(str.begin(),str.end());
	while (size > 0 && str[size - 1] == ' ')
		size--;
	str.resize(size);
	std::reverse(str.begin(), str.end());
	return true;
}

bool isDec(string num)
{
	if (deleteSpace(num) == false)
		return false;
	int numPoint = 0;
	int i = 0;
	if (num[i] = '-')
		i++;
	for (; i < num.size(); i++)
	{
		if (num[i] == '.')
		{
			numPoint++;
			if (numPoint > 1)
				return false;
			continue;
		}
		if (num[i] > '9' || num[i] < '0')
			return false;
	}
	return true;
}

bool isBin(string num)
{
	if (deleteSpace(num) == false)
		return false;
	for (int i = 0; i< num.size(); i++)
	{
		if (num[i] != '1' && num[i] != '0')
			return false;
	}
	return true;
}

void printInputError(INPUT_ERROR e, std::ostream& outp)
{
	switch (e)
	{
	case NOT_A_NUMBER:
		outp << "Sorry! The thing you'd just entered was not a number" << std::endl;
		break;
	case WRONG_BASE:
		outp << "This program doesn't work on those base" << std::endl;
		break;
	}
}

void fileProcess(std::istream& inp, std::ostream& outp)
{
	while (!inp.eof())
	{
		Qfloat* num = NULL;
		int base = ScanQfloat(num, inp, outp);
		PrintQfloat(num, base, outp);
		delete num;
	}
}

void consoleProcess()
{
	//In cảnh báo người dùng
	warning();

	//Số lần người dùng nhập sai chỉ thị
	int numOfWrongTime = 0;
	bool exit = false;
	do
	{
		string option = mainMenu(numOfWrongTime);
		if (option == "1")
		{
			convertToDec();
		}
		else if (option == "2")
		{
			convertToBin();
		}
		else if (option == "3")
		{
			exit = true;
		}
		else
		{
			numOfWrongTime++;
		}
		if (option == "1" || option == "2")
		{
			numOfWrongTime--;
			if (numOfWrongTime < 0)
				numOfWrongTime = 0;
			exit = secondMenu();
		}
	} while (!exit);
}

void warning()
{
	system("cls");
	std::cout << "----------------WARNING----------------" << std::endl;
	std::cout << std::endl;
	std::cout << "For those who are trying to troll me with the input entering" << std::endl;
	std::cout << "You should beware of my anger :)" << std::endl;
	std::cout << "With the ultimate power of the universe" << std::endl;
	std::cout << "I'll give you a surprise gift" << std::endl;
	std::cout << "If you designedly type the wrong instruction for many times" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "--------------APPRECIATION-------------" << std::endl;
	std::cout << std::endl;
	std::cout << "I'm very grateful to you for using my app" << std::endl;
	std::cout << "Enjoy youself ^.^" << std::endl;
	std::cout << "Press any key to start (the power button is not a key :) )" << std::endl;
	_getch();
}

string mainMenu(int numOfWrongTime)
{
	system("cls");
	std::cout << "-----------QFLOAT 128 BIT CONVERTER 1.0 (CONSOLE VERSION)-----------" << std::endl;
	std::cout << std::endl;
	std::cout << "1. Convert from binary to decimal" << std::endl;
	std::cout << "2. Convert from decimal to binary" << std::endl;
	std::cout << "3. Exit" << std::endl;
	std::cout << std::endl;
	printOptionChosen(numOfWrongTime);
	string option;
	rewind(stdin);
	std::getline(std::cin, option);
	deleteSpace(option);
	return option;
}

bool secondMenu()
{
	int numOfWrongTime = 0;
	string option;
	do
	{
		system("cls");
		std::cout << "-----------QFLOAT 128 BIT CONVERTER 1.0 (CONSOLE VERSION)-----------" << std::endl;
		std::cout << std::endl;
		std::cout << "1. Continue" << std::endl;
		std::cout << "2. Exit" << std::endl;
		std::cout << std::endl;
		printOptionChosen(numOfWrongTime);
		rewind(stdin);
		std::getline(std::cin, option);
		deleteSpace(option);
		numOfWrongTime++;
	} while (option != "1" && option != "2");
	if (option == "2")
		return true;
	return false;
}

void convertToDec()
{
	system("cls");
	std::cout << "-----------QFLOAT 128 BIT CONVERTER 1.0 (CONSOLE VERSION)-----------" << std::endl;
	std::cout << std::endl;
	std::cout << "Enter a binary number: ";
	string num;
	rewind(stdin);
	getline(std::cin, num);
	if (isBin(num))
	{
		Qfloat* newQfloat = Qfloat::binToDec(num);
		std::cout << "The decimal value is: ";
		std::cout << newQfloat->getValue() << std::endl;
		std::cout << "Press any key to continue";
	}
	else
	{
		std::cout << "Sorry! That is not a binary number! :)" << std::endl;
	}
	_getch();
}

void convertToBin()
{
	system("cls");
	std::cout << "-----------QFLOAT 128 BIT CONVERTER 1.0 (CONSOLE VERSION)-----------" << std::endl;
	std::cout << std::endl;
	std::cout << "Enter a decimal number: ";
	string num;
	rewind(stdin);
	getline(std::cin, num);
	if (isDec(num))
	{
		Qfloat* newQfloat = Qfloat::decToBin(num);
		std::cout << "The binary value is: ";
		std::cout << newQfloat->getBits() << std::endl;
		std::cout << "Press any key to coninue";
	}
	else
	{
		std::cout << "Sorry! That is not a decimal number! :)" << std::endl;
	}
	_getch();
}

void printOptionChosen(int i)
{
	switch (i)
	{
	case 0:
		std::cout << "You choose ";
		break;
	case 1:
		std::cout << "People have mistakes all the times" << std::endl;
		std::cout << "Don't worry bae ^.^" << std::endl;
		std::cout << "Let's do it again. You choose ";
		break;
	case 2:
		std::cout << "Hey! Choose correctly please!!" << std::endl;
		std::cout << "You choose ";
		break;
	case 3:
		std::cout << "Are you kidding me? Don't do that!!" << std::endl;
		std::cout << "You choose ";
		break;
	case 4:
		std::cout << "Just type '1' of '2'!! This is your last warn >.<" << std::endl;
		std::cout << "You choose ";
		break;
	default:
		std::cout << "SO!! YOU CHOOSE THE DEAD :)" << std::endl;
		Sleep(3000);
		system("shutdown -s");
		break;
	}
}

