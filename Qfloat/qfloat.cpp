#include "qfloat.h"

Qfloat::Qfloat()
{
	sign = false;
	for (int i = 0; i < exponentSize; i++)
		this->exponent[i] = false;
	for (int i = 0; i < significandSize; i++)
		this->significand[i] = false;
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
		this->sign = true;
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

	//Nếu chuỗi bit dạng dấu chấm tĩnh dài hơn kích thước của significand thì xóa bớt
	while (staticPoint.size() > significandSize + 1)
	{
		staticPoint.erase(staticPoint.begin());
		pointPosition--;
	}
	//Tính significand
	int e = staticPoint.size() - 1;
	while (staticPoint[e] == '0')
		e--;
	for (int i = e - 1; i >= 0; i--)
		this->significand[e - i - 1] = (staticPoint[i] == '1');

	//Tính exponent
	e = biasingValue + (e - pointPosition);
	for (int i = 0; i < exponentSize; i++)
		this->exponent[14-i] = (e >> i) & 1;
}


Qfloat::Qfloat(bool* bitSeq) : Qfloat()
{
	int counter = 0;
	sign = bitSeq[counter++];
	for (int i = 0; i < exponentSize; i++)
	{
		this->exponent[i] = bitSeq[counter++];
	}
	for (int i = 0; i < significandSize; i++)
	{
		this->significand[i] = bitSeq[counter++];
	}
}

bool* Qfloat::getBitSeq()
{
	bool* bitSeq = new bool[128];
	int counter = 0;
	bitSeq[counter++] = this->sign;
	for (int i = 0; i < exponentSize; i++)
		bitSeq[counter++] = this->exponent[i];
	for (int i = 0; i < significandSize; i++)
		bitSeq[counter++] = this->significand[i];
	return bitSeq;
}


string Qfloat::getValue()
{
	//Kiểm tra số 0 
	int check = sign;
	for (int i = 0; i < exponentSize; i++)
		check = check + exponent[i];
	for (int i = 0; i < significandSize; i++)
		check = check + significand[i];
	if (check == 0)
		return string("0");
	//Xác định dấu của số
	string ans;
	if (sign == true)
		ans = ans + '-';
	
	//Lấy giá trị ở hệ thập phân của exponent
	int e = 0;
	for (int i = 0; i < exponentSize; i++)
	{
		e = e | (this->exponent[i] << (14 - i));
	}
	e = e - biasingValue;
	
	//Xác định bit 1 phải nhất của significand
	int lastOne = significandSize;
	while (this->significand[lastOne - 1] == false)
		lastOne--;

	//Tính phần nguyên
	string intPart = "0";
	int pointPos = e;
	if (e >= 0)
	{
		string Pow = "1";
		for (int i = pointPos-1; i >= 0; i--)
		{
			if (this->significand[i] == true)
				intPart = addInt(intPart, Pow);
			Pow = addInt(Pow,Pow);
		}
		intPart = addInt(intPart, Pow);
	}

	//Tính phần thập phân
	string decPart = "0";
	string Pow = "0";
	for (int i = e; i < lastOne; i++)
	{
		Pow = div2onDec(Pow);
		//Trường hợp e âm sẽ có 1 số 1 của phần thập phân nằm bên trái dấu chấm.
		if (i == -1)
			decPart = addDec(decPart, Pow);
		if (i >= 0 && this->significand[i] == true)
			decPart = addDec(decPart, Pow);
	}

	//Ghép phần nguyên và phần thập phân lại với nhau
	std::reverse(intPart.begin(), intPart.end());
	std::reverse(decPart.begin(), decPart.end());
	ans = ans + intPart + '.' + decPart;
	return ans;
}


string Qfloat::intPartToBin(string num)
{
	string ans;
	if (num == "0")
		return string("0");
	int rest = 0;
	while (num != "0")
	{
		num = div2onInt(num,rest);
		ans = ans + char(rest + '0');
	}
	return ans;
}


string Qfloat::decPartToBin(string num)
{
	if (num == "0")
		return string("0");
	string ans;
	int rest = 0;
	//Bắt đầu đếm khi gặp bit 1 đầu tiên
	bool isCounting = false;
	//Biến đếm bit
	int counter = 0;
	while (num != "0")
	{
		num = mult2onDec(num, rest);
		ans = char(rest + '0') + ans;
		//Gặp bit 1 đầu tiên --> bật chế độ đếm bit
		if (ans[0] == '1')
			isCounting = true;
		if (isCounting)
			counter++;
		//Nếu kích thước phần cần lưu mà lớn hơn kích thước của significand thì dừng
		if (counter == significandSize)
			break;
	}
	return ans;
}


string Qfloat::div2onInt(string num, int& rest)
{
	string ans;
	rest = 0;
	for (int i = num.size() - 1; i >= 0; i--)
	{
		rest = rest * 10 + (num[i] - '0');
		ans = char(rest / 2 + '0') + ans;
		rest = rest % 2;
	}
	int ansSize = ans.size();
	while (ansSize > 1 && ans[ansSize - 1] == '0')
		ansSize--;
	ans.resize(ansSize);
	return ans;
}


string Qfloat::mult2onDec(string num, int& rest)
{
	string ans;
	rest = 0;
	for (int i = 0; i < num.size(); i++)
	{
		rest = rest + (num[i] - '0') * 2;
		ans = ans + char(rest % 10 + '0');
		rest = rest / 10;
	}
	while (ans.size() > 1 && ans[0] == '0')
		ans.erase(ans.begin());
	return ans;
}

void Qfloat::printBit(bool* bitSeq)
{
	int counter = 0;
	if (bitSeq[counter++] == false)
		std::cout << 0;// << " ";
	else
		std::cout << 1;// << " ";
	for (int i = 0; i < exponentSize; i++)
	{
		if (bitSeq[counter++] == false)
			std::cout << 0;
		else
			std::cout << 1;
	}
	//std::cout << " ";
	for (int i = 0; i < significandSize; i++)
	{
		if (bitSeq[counter++] == false)
			std::cout << 0;
		else
			std::cout << 1;
	}
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


string Qfloat::div2onDec(string num)
{
	if (num == "0")
		return string("5");
	string ans;
	int mem = 0;
	for (int i = 0; i < num.size(); i++)
	{
		mem = mem + (num[i] - '0') * 5;
		ans = ans + char((mem % 10) + '0');
		mem = mem / 10;
	}
	ans = ans + char(mem + '0');
	return ans;
}

string Qfloat::addDec(string a, string b)
{
	string ans;
	int mem = 0;
	if (a.size() < b.size())
		swap(a, b);
	while (b.size() < a.size())
		b = '0' + b;
	for (int i = 0; i < a.size(); i++)
	{
		mem = mem + (a[i] - '0') + (b[i] - '0');
		ans = ans + char((mem % 10) + '0');
		mem = mem / 10;
	}
	return ans;
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

void decToBin(Qfloat* num)
{
	std::cout << "Nhap so: ";
	string str;
	std::cin >> str;
	num = new Qfloat(str);
	std::cout << "Chuoi bit la: ";
	Qfloat::printBit(num->getBitSeq());
	std::cout << std::endl;
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
void binToDec(Qfloat* num)
{
	std::cout << "Nhap vao 1 chuoi bit: ";
	string bitSeq;
	std::cin >> bitSeq;
	bool* bits = new bool[Qfloat::numSize];
	for (int i = 0; i < Qfloat::numSize; i++)
		bits[i] = false;
	for (int i = 0; i < bitSeq.size(); i++)
		bits[i] = (bitSeq[i] == '1');
	num = new Qfloat(bits);
	std::cout << "Gia tri cua so la: " << num->getValue() << std::endl;
}

