﻿#ifndef __QFLOAT__
#define __QFLOAT__
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include "bigint.h"
using std::string;

/*
Các tham nhận diện lỗi nhập dữ liệu của người dùng
	+NOT_A_NUMBER: chuỗi số vừa nhập không phải là 1 số 
	+WRONG_BASE: Hệ số chuyển đổi chưa được hỗ trợ
*/
enum INPUT_ERROR
{
	NOT_A_NUMBER,
	WRONG_BASE,
};

/*
lỚP MÔ TẢ CÁC ĐÔI TƯỢNG SỐ THỰC LƯU DƯỚI DẠNG SỐ CHẤM ĐỘNG KÍCH THƯỚC 128BIT
	+Sign: 1 bit
	+Exponent: 15 bit
	+Significand: 112 bit

INPUT1: 1 chuỗi biểu diễn 1 số thực ở hệ thập phân
OUTPUT1: 1 Chuỗi bit là biểu diễn nhị phân ở dạng số chấm động của số thực

INPUT2: 1 chuỗi bit 
OUTPUT2:  1 chuỗi biểu diễn giá trị ở hệ thập phân của chuỗi bit

TODO:
	+Gán tên cho các hằng số(Tổng quát hóa)
	+Hàm khởi tạo mặc định: Giá trị của dãy bit toàn số 0 
	+Hàm khởi tạo tham số 1: Đưa vào chuỗi biểu diễn số thập phân
	+Hàm khởi tạo tham số 2: Đưa vào mảng bool biểu diễn chuỗi bit.
	+Hàm xuất giá trị thập phân: Trả về 1 chuỗi mang giá trị thập phân của số
	+Hàm xuất giá trị nhị phân: Trả về 1 mảng bool thể hiện giá trị của số ở hệ nhị phân
*/
class Qfloat
{
private:
	static const int exponentSize = 15;
	static const int significandSize = 112;
	static const int biasingValue = 16383;
	
	//		[0..31]    [32..63]    [64..95]    [96..128]	
	int data[4];

	//Hàm lấy bit dấu
	bool getSign();

	//Hàm lấy bit thứ i của exponent
	bool getExponent(int i);
	
	//Hàm lấy bit thứ i của significand
	bool getSignificand(int i);

	//Hàm gán bit dấu
	void setSign(bool value);

	//Hàm gán bit thứ i của exponent
	void setExponent(int i, bool value);

	//Hàm gán bit thứu i của signficand
	void setSignificand(int i, bool value);
public:
	static const int numSize = 128;
	//CONSTRUCTOR
	/*
	Hàm khởi tạo mặc định:
	Tất cả các bit đều bằng 0
	*/
	Qfloat();	//DONE
	
	/*
	Hàm khởi tạo đưa vào 1 số thập phân dạng chuỗi.
	TODO:
		+Chuẩn hóa 1 số để tránh người dùng nhập không chuẩn
		+Kiểm tra số vừa nhập bằng 0
		+Kiểm tra số âm
		+Tách phần nguyên(integer part) và phần thập phân(decimal part)
		+Chuyển phân nguyên và phần thập phân thành dãy bit dạng số dấu chấm tĩnh.
		+Tính significand
		+Tính exponent
	*/
	Qfloat(string);
	
	//Hàm dựng từ 1 mảng bool biểu diễn giá trị ở hệ nhị phân của 1 số
	Qfloat(bool* bitSeq);	//DONE

	//getter
	//Hàm xuất ra chuỗi bit của đối tượng
	string getBits();	//DONE
	
	/*
	Hàm chuyển đổi dãy bit thành số thập phân
	TODO:
		+Kiểm tra số đặc biệt
		+Xác định dấu của số
		+Lấy giá trị ở hệ thập phân của exponent
		+Xác định bit 1 phải nhất của significand
		+Tính phần nguyên
		+Tính phần thập phân
		+Ghép phần nguyên và phần thập phân lại với nhau
	*/
	string getValue();

	//Chuyển chuỗi num ở hệ thập phân thành 1 đối tượng Qfloat
	static Qfloat* decToBin(string num);

	/*
	Chuyển chuỗi num ở hệ nhị phân thành 1 đối tượng Qfloat
	TODO:
		+Thêm các bit 0 vào đầu chuỗi nếu chưa ddue 128 bit
		+Tạo đối tượng Qfloat
	*/
	static Qfloat* binToDec(string num);

	//ADDITIONAL FUNCTION
	//Chuyển phần nguyên của của số thực sang dãy nhị phân
	static string intPartToBin(string);	//DONE UPDATE BIGNUM

	/*
	Chuyển phần thực của số thực sang dãy nhị phân
	TODO:
		+Xử lý phần thực là số 0
		+Xử lý các số vượt quá số bit để lưu
	*/
	static string decPartToBin(string);  //DONE UPDATE BIGNUM
	
	/*
	Nhận vào 1 số nguyên lớn(num) và 1 biến char (rest)
	Trả về thương sau khi chia 2 và phần dư được lưu vào rest
	*/
	static void div2onInt(BigInt&, char&);	//DONE UPDATE BIGNUM
	
	/*
	Nhận vào phần thực của 1 số(num) và biến số nguyên(real size) là kích thước thực tế của phần thập phân (tính thêm những số 0 ở đầu)
	Kết quả trả về phần thực của phép num*2
	rest = '1' nếu num*2 > 1 và ngược lại
	*/
	static void mult2onDec(BigInt&, int, char&); //DONE

	//Hàm cộng phần nguyên của 2 số
	static string addInt(string, string); //UNUSED FUNCTION
	
	/*
	Hàm chia 2 ở phần thập phân của 1 số(num)
	TODO:
		+Nhân phần thập phân đó cho 5	
		+Tăng kích thước thực tế (Do x*0.5 sẽ thêm 1 chữ số sau dấu '.')
	*/
	static void div2onDec(BigInt&,int &);	


	//Cộng phần thập phân của 2 số
	static void addDec(BigInt&, int& , BigInt&, int&);	//DONE UPDATE BIGNUM
	
	/*
	Chuẩn hóa 1 chuỗi sô do người dùng nhập
	TODO:
		+Tìm vị trí của dấu chấm
		+Thêm vào phần thập phân = 0 nếu không có
		+Xóa đi những số 0 vô nghĩa của phần thập phân
		+Đảo chuỗi để tiện xử lý
		+Xóa đi những số 0 vô nghĩa của phần nguyên
		+Nếu số được nhập là -0.0 thì chuyển thành 0.0
		+Đảo chuỗi lại
	*/
	static string standardize(string);	

	//Các hàm kiểm tra các số đặc biệt:
	//Số 0: exponent == 0 && significand == 0
	bool isZero();
	
	//Số không thể chuẩn hóa(Denormalized Number): exponent == 0 && significand != 0
	bool isDenormalized();
	
	//Sô vô cùng(Inf): exponent == 111..1 && significand == 0
	bool isInf();
	
	//Số báo lỗi(NaN): exponent == 111..1 && significand != 0
	bool isNaN();
};

//USER DISPLAY FUNCTION

/*
Nhập dữ liệu
TODO:
	+Nhập 2 chỉ thị p1, p2 và chuỗi số
	+Kiểm tra chuỗi số vừa nhập có phải là 1 con số hay không?
	+Kiểm tra 2 chỉ thị có ở ngoài 2 hệ 10 và 2 không
	+Chuyển chuỗi số vừa nhập được thành 1 đối tượng Qfloat
	+Trả về giá trị của p2
*/
int ScanQfloat(Qfloat *&,std::istream&,std::ostream&);

//Xuất num ra màn hình ở hệ base
void PrintQfloat(Qfloat* num, int base, std::ostream&);

//Hàm xóa khoảng trống ở 2 đầu của 1 chuỗi, trả về false nếu chuỗi rỗng
bool deleteSpace(string& str);

/*
Kiểm tra chuỗi num có phải là 1 số hay không?
TODO:
	+Xóa khoảng trống đầu và cuối chuỗi
	+Kiểm tra các kí tự chỉ gồm:
		+'0' -> '9'
		+'.'
	+Số lượng kí tự '.' < 2
*/
bool isDec(string num);

//Kiểm tra chuỗi num có phải là 1 dãy bit hay không
bool isBin(string num);

//Xóa các kí tự thừa còn trong bộ nhớ tạm

//In ra lỗi nhập liệu
void printInputError(INPUT_ERROR,std::ostream &);

//Flow chính của cả chương trình: Được mô tả theo đề bài yêu cầu
void fileProcess(std::istream&, std::ostream&);

/*
Flow phụ nhập xuất trong console hỗ trợ cho việc testing
TODO:
	+Tạo bảng cảnh báo người dùng (Hù dạo) =)))
	+Tạo biến lưu số lần người dùng nhập sai
		+Người dùng nhập đúng -> giảm giá trị xuống 1 đơn vị. Tối thiểu là 0
		+Người dùng nhập sai  -> tăng giá trị lên 1 đơn vị. 
	+Menu chính gồm có 3 chức năng:
		+1.Chuyển nhị phân thành thập phân
		+2.Chuyển thập phân thành nhị phân
		+3.Thoát chương trình
	+Nếu người dùng chọn chức nặng 1 hoặc 2, sau khi thực hiện xong sẽ hiện menu phụ
	-Menu phụ gồm 2 chức năng:
		-1.Tiếp tục(Quay lại menu chính)
		-2.Thoát chương trình
*/
void consoleProcess();

//Cảnh báo người dùng: ĐỪNG CÓ TROLL DEV :)
void warning();

//Menu chính: Đưa vào số lần người dùng nhập sai (Nếu sai nhiều quá -> CHỬI CM người dùng)
string mainMenu(int numOfWrongTime);

//Menu phụ: Trả về true nếu người dùng muốn thoát chương trình và ngược lại
bool secondMenu();

//Giao diện chức năng chuyển nhị phân -> thập phân
void convertToDec();

//Giao diện chức năng chuyển thập phân -> nhị phân
void convertToBin();

//CHỬI NGƯỜI DÙNG
void printOptionChosen(int i);
#endif // !__QFLOAT__

/*
CÁC HẠN CHẾ CHƯA GIẢI QUYẾT ĐƯỢC
	-Chưa có hàm làm tròn và quy định về việc làm tròn mặc định bao nhiêu chữ số ở phần thập phân
	-Chưa tính được giá trị của denormalized number.
	-Chưa hoàn thiện tất cả các hàm của BigInt.
	-Nên định nghĩa lại BigFloat kế thừa từ BigInt để tính toán đỡ rối hơn.
*/

