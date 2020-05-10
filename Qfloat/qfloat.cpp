#include "qfloat.h"

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
		this->setSignificand(e - i - 1, staticPoint[i] == '1');

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
	string intPart = "0";
	int pointPos = e;
	if (e >= 0)
	{
		string Pow = "1";
		for (int i = pointPos-1; i >= 0; i--)
		{
			if (this->getSignificand(i) == true)
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
		if (i >= 0 && this->getSignificand(i) == true)
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

Qfloat* decToBin(string num)
{
	Qfloat* newQfloat = new Qfloat(num);
	return newQfloat;
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
Qfloat* binToDec(string num)
{
	bool* bits = new bool[128];
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
	inp >> p1 >> p2;
	std::getline(inp,str);

	//Kiểm tra chuỗi số vừa nhập có phải là con số hay không?
	if (isNum(str) == false)
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
		num = binToDec(str);
	else
		num = decToBin(str);
	
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

bool isNum(string& num)
{
	//Kiểm tra các kí tự có hợp lệ không:
	for (int i = 0; i < num.size(); i++)
		if (!(num[i] == ' ' ||
			(num[i] >= '0' && num[i] <= '9') ||
			num[i] == '.'))
		{
			return false;
		}

	//Xóa các khoảng trắng ở đầu và cuối chuỗi để tiện kiểm tra
	std::reverse(num.begin(), num.end());
	int size = num.size();
	while (num.size() > 0 && num[size - 1] == ' ')
		size--;
	num.resize(size);
	std::reverse(num.begin(), num.end());
	size = num.size();
	while (num.size() > 0 && num[size - 1] == ' ')
		size--;
	num.resize(size);

	//Kiểm tra có kí tự ' ' nào ở giữa chuỗi không
	for (int i = 0; i < num.size(); i++)
	{
		if (num[i] == ' ')
			return false;
	}
	
	//Kiểm tra số lượng kí tự '.' trong chuỗi
	int numPoint = 0;
	for (int i = 0; i < num.size(); i++)
	{
		if (num[i] == '.')
			numPoint++;
		if (numPoint > 1)
			return false;
	}
	
	//Kiểm tra chuỗi rỗng
	if (num.size() == 0)
		return false;
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

void process(std::istream& inp, std::ostream& outp)
{
	while (!inp.eof())
	{
		Qfloat* num = NULL;
		int base = ScanQfloat(num, inp, outp);
		PrintQfloat(num, base, outp);
		delete num;
	}
}

