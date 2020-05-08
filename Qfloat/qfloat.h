#ifndef __QFLOAT__
#define __QFLOAT__
#include <iostream>
#include <algorithm>
#include <string>
using std::string;

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
	bool sign;
	bool exponent[15];
	bool significand[112];
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
		+Chuẩn hóa 1 số để tránh người dùng nhập không chuẩn và đảo chuỗi để tiện xử lý
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
	//Hàm xuất ra mảng bool là chuỗi bit thể hiện dạng số chấm động của đối tượng
	bool* getBitSeq();	//DONE
	
	/*
	Hàm chuyển đổi dãy bit thành số thập phân
	TODO:
		+Kiểm tra số 0
		+Xác định dấu của số
		+Lấy giá trị ở hệ thập phân của exponent
		+Xác định bit 1 phải nhất của significand
		+Tính phần nguyên
		+Tính phần thập phân
		+Ghép phần nguyên và phần thập phân lại với nhau
	*/
	string getValue();

	//ADDITIONAL FUNCTION
	//Chuyển phần nguyên của của số thực sang dãy nhị phân
	static string intPartToBin(string);	//DONE

	/*
	Chuyển phần thực của số thực sang dãy nhị phân
	TODO:
		+Xử lý phần thực là số 0
		+Xử lý các số vượt quá số bit để lưu
	*/
	static string decPartToBin(string); 
	
	/*
	Nhận vào 1 số nguyên chuỗi(num) và 1 biến số nguyên(rest)
	Trả về thương sau khi chia 2 và phần dư được lưu vào rest
	*/
	static string div2onInt(string, int&);	//DONE
	
	/*
	Nhận vào phần thực của 1 số(num) và biến số nguyên(rest)
	Kết quả trả về phần thực của phép num*2
	rest = 1 nếu num*2 > 1 và ngược lại
	*/
	static string mult2onDec(string, int&); //DONE
	
	//Xuất chuỗi bit ra màn hình từ 1 mảng bool theo định dạng dấu chấm động
	static void printBit(bool*); //DONE

	//Hàm cộng phần nguyên của 2 số
	static string addInt(string, string); //DONE
	
	/*
	Hàm chia 2 ở phần thập phân của 1 số(num)
	TODO:
		+Trường hợp num = 0 thì kết quả trả về là 5
		+Nhân phần thập phân đó cho 5	
		+nếu số lượng chữ số của kết quả = số chữ số của num thì thêm 1 chữ số 0 vào đầu
	*/
	static string div2onDec(string);	


	//Cộng phần thập phân của 2 số
	static string addDec(string, string);	//DONE
	
	/*
	Chuẩn hóa 1 chuỗi sô do người dùng nhập
	TODO:
		+Tìm vị trí của dấu chấm
		+Thêm vào phần thập phân = 0 nếu không có
		+Xóa đi những số 0 vô nghĩa của phần thập phân
		+Đảo chuỗi để tiện xử lý
		+Xóa đi những số 0 vô nghĩa của phần nguyên
		+Nếu số được nhập là -0.0 thì chuyển thành 0.0
	*/
	static string standardize(string);	
};

//USER DISPLAY FUNCTION
/*
Giao diện chuyển từ thập phân sang nhị phân
TODO:
	+Nhập 1 số thập phân 
	+Chuyển sang chuỗi nhị phân 
	+Xuất chuỗi nhị phân ra màn hình
*/
void decToBin(Qfloat* num);

/*
Giao diện chuyển từ nhị phân sang thập phân
TODO:
	+Nhập 1 chuỗi bit
	+Chuyển chuỗi bit thành mảng boot có Qfloat::numSize phần tử
	+Lưu vào biến kiểu Qfloat
	+Xuất giá trị thập phân của số ra màn hình
*/
void binToDec(Qfloat* num);
#endif // !__QFLOAT__

/*
CÁC HẠN CHẾ CHƯA GIẢI QUYẾT ĐƯỢC
	-Thời gian tính toán quá lâu nếu nhập 1 dãy bit của số quá lớn hoặc quá nhỏ
	-Chưa có hàm làm tròn và quy định về việc làm tròn mặc định bao nhiêu chữ số ở phần thập phân
	-Chưa có các đối tượng đặc biệt như số vô cùng (Inf), số báo lỗi(NaN), Denomalize number
	-Chưa có các hàm giao diện thân thiện với người dùng
	-Chưa có các toán tử số học, gán, so sánh cho đối tượng
*/

/*
CÁC GỢI Ý HƯỚNG GIẢI QUYẾT
	-Dùng hàm nhân nhanh 2 số lớn (FFT, katastuba)
	-Đặt số lượng chữ số tối đa của phần thập phân và phần nguyên <= 10000	
*/