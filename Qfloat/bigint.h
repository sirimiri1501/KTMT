#ifndef __BIGINT__
#define __BIGINT__
#include<algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

/*
Class định nghĩa các đối tượng số lớn có kích thước tối đa khoảng 10^8 chữ số
Số lớn được biểu diễn ở hệ 10^9 (Có thể tùy biến lại hệ trong tương lai).
Class gồm có: Mảng các biến kiểu int, mỗi phần tử được xem như 1 "chữ số" của 1 số lớn
Các lưu trữ: Các "chữ số" sẽ được lưu ngược với tự nhiên, các chữ số mang giá trị lớn sẽ nằm sau các chữ số mang giá trị nhỏ (Tiện cho việc tính toán)
Các phép toán hỗ trợ:
	-Chuyển 1 số nhỏ thành số lớn.
	-Cộng 2 số lớn.
	-Trừ 2 số lớn
	-Nhân số lớn với số nhỏ.
	-Chia lấy phần nguyên số lớn với số nhỏ.
	-Chia lấy phần dư số lớn với số nhỏ.
	-Nhân số lớn với số lớn.
	-Chia số lớn với số lớn.
	-Gán số nhỏ cho số lớn
	-Gán số lớn cho số lớn
	-Nhập xuất số lớn
*/

class BigInt
{
private:
	int* data;
	//Số lượng chữ số thật sự 
	int size;
public:
	static const int maxSize = 2e3;
	static const int base = 1e9;

	int getSize();
	
	//CONSTRUCTOR
	//Mặc định giá trị là 0
	BigInt();	//DONE

	//Chuyển 1 số nhỏ thành số lớn
	BigInt(int);	//DONE

	//Chuyển 1 chuỗi thành số lớn
	BigInt(std::string);	//DONE

	//Copy constructor
	BigInt(const BigInt&);	//DONE

	//Operator
	//Hàm cộng 2 số lớn, trả về số lớn
	friend BigInt operator+(const BigInt& a, const BigInt& b);
	
	//Hàm trừ 2 số lớn, trả về số lớn
	friend BigInt operator-(const BigInt& a, const BigInt& b);
	
	//Hàm nhân số lớn với số nhỏ, trả về số lớn
	const BigInt operator*(int) const;

	//Hàm chia lấy phần nguyên số lớn với số nhỏ, trả về số lớn
	friend BigInt operator/(const BigInt& a, int b);

	//Hàm chia lấy phần dư số lớn với số nhỏ, trả về số nhỏ
	int operator%(int b);

	//Hàm gán số nhỏ cho số lớn
	BigInt& operator=(int);

	//Hàm gán số lớn cho số lớn
	BigInt& operator=(const BigInt& a);

	//Hàm nhập số lớn
	friend std::istream& operator>>(std::istream& input, BigInt&);

	//Hàm xuất số lớn
	friend std::ostream& operator<<(std::ostream& output, const BigInt&);
	
	//debug function
	void debug();
	//DESTRUCTOR
	~BigInt();

	//addition function
	static int numOfDigits(int x);
	int arraySize();

	std::string getNum();

	void resize(int size);

	bool isZero();
};

#endif