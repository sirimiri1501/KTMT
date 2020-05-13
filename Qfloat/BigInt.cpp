#include "bigint.h"

int BigInt::getSize()
{
	return size;
}

BigInt::BigInt()
{
	data = new int[maxSize];
	for (int i = 0; i < maxSize; i++)
		data[i] = 0;
	size = 1;
}

BigInt::BigInt(int num) : BigInt()
{
	data[0] = num;
	size = numOfDigits(num);
}

BigInt::BigInt(std::string num) : BigInt()
{
	int temp = 0;
	std::reverse(num.begin(), num.end());
	while (num.size() > 1 && num[num.size() - 1] == '0')
		num.resize(num.size() - 1);
	for (int i = num.size() - 1; i >= 0; i--)
	{
		temp = temp * 10 + (num[i] - '0');
		if (i % 9 == 0)	//Tại các chữ số ở vị trí chia hết cho 9 sẽ là chữ số đơn vị của phần tử i/9
		{
			data[i/9] = temp;
			temp = 0;
		}
	}
	size = num.size();
}

BigInt::BigInt(const BigInt& source) : BigInt()
{
	size = source.size;
	for (int i = 0; i < (source.size + 8) / 9; i++)
	{
		data[i] = source.data[i];
	}
}

BigInt& BigInt::operator=(int)
{
	// TODO: insert return statement here
	return *this;
}

BigInt& BigInt::operator=(const BigInt& source)
{
	if (this != &source)
	{
		size = source.size;
		for (int i = 0; i < (source.size + 8) / 9; i++)
			data[i] = source.data[i];
	}
	return *this;
}

void BigInt::debug()
{
	for (int i = 0; i < (size + 8) / 9; i++)	//Nếu size % 9 khác 0 sẽ + 1 đơn vị vào
		std::cout << data[i] << " ";
	std::cout << std::endl << size << std::endl;
}

BigInt::~BigInt()
{
	if (data == NULL)
		return;
	delete[] data;
	size = 0;
}

int BigInt::numOfDigits(int x)
{
	if (x == 0)
		return 1;
	return int(log10(x) + 1);
}

int BigInt::arraySize()
{
	return (size + 8) / 9;
}

std::string BigInt::getNum()
{
	std::string ans;
	//Xử lý phần tử cao nhất
	if (data[arraySize() - 1] == 0)
	{
		for (int i = 0; i < size - (arraySize() - 1) * 9; i++)
			ans = ans + '0';
	}
	else
	{
		for (int i = numOfDigits(data[arraySize() - 1]); i < size - (arraySize() - 1) * 9; i++)
			ans = ans + '0';
		ans = ans + std::to_string(data[arraySize() - 1]);
	}
	//Các phần tử còn lại
	for (int i = arraySize() - 2; i >= 0; i--)
	{
		if (data[i] == 0)
			ans = ans + "000000000";
		else
		{
			for (int j = numOfDigits(data[i]); j < 9; j++)
				ans = ans + '0';
			ans = ans + std::to_string(data[i]);
		}
	}
	std::reverse(ans.begin(),ans.end());
	while (size < ans.size())
		ans.erase(ans.end() - 1);
	while (size > ans.size())
		ans = ans + '0';
	std::reverse(ans.begin(), ans.end());
	return ans;
}

void BigInt::resize(int newSize)
{
	if(newSize < size)
	{
		//Nếu số lượng chữ số của phần tử cuối cùng nhiều hơn số lượng muốn resize 
		//Thì bỏ những chữ số đó
		//Bằng các % 10^x
		//x là số lượng chữ số mong muốn
		int pow = 1;
		for (int i = 1; i <= newSize - (arraySize() - 1) * 9; i++)
			pow = pow * 10;
		data[arraySize() - 1] %= pow;
	}
	size = newSize;
}

bool BigInt::isZero()
{
	if (size == 1 && data[0] == 0)
		return true;
	return false;
}

BigInt operator+(const BigInt& a, const BigInt& b)
{
	BigInt c;
	c.size = std::max(a.size, b.size);
	int memo = 0;
	for (int i = 0; i < c.arraySize(); i++)
	{
		memo = memo + a.data[i] + b.data[i];
		c.data[i] = memo % BigInt::base;
		memo = memo / BigInt::base;
	}
	if (memo > 0)
	{
		c.size = (c.arraySize() - 1) * 9 + 9 + BigInt::numOfDigits(memo);
		c.data[c.arraySize() - 1] = memo;
	}
	else
	{
		c.size = (c.arraySize() - 1) * 9 + BigInt::numOfDigits(c.data[c.arraySize() - 1]);
	}
	return c;
}

BigInt operator-(const BigInt& a, const BigInt& b)
{
	return BigInt();
}

const BigInt BigInt::operator*(int b) const 
{
	BigInt c;
	c.size = size;
	long long memo = 0;
	int arraySize = c.arraySize();
	for (int i = 0; i < arraySize; i++)
	{ 
		memo = memo + long long(data[i]) * long long(b);
		long long tmp = memo % BigInt::base;
		c.data[i] = tmp;
		memo = memo / BigInt::base;
	}
	if (memo > 0)
	{
		c.size = (arraySize - 1) * 9 + 9 + BigInt::numOfDigits(memo);
		c.data[c.arraySize() - 1] = memo;
	}
	else
	{
		c.size = (arraySize - 1) * 9 + BigInt::numOfDigits(c.data[arraySize - 1]);
	}
	return c;
}

BigInt operator/(const BigInt& a, int b)
{
	return BigInt();
}

int BigInt::operator%(int b)
{
	long long memo = 0;
	for (int i = arraySize() - 1; i >= 0; i--)
	{
		memo = memo * BigInt::base + data[i];
		long long tmp = memo / b;
		data[i] = tmp;
		memo = memo % b;
	}
	while (size > 0 && data[arraySize() - 1] == 0)
	{
		size = (arraySize() - 1) * 9;
	}
	if (size == 0)
		size = 1;
	return memo;
}

std::istream& operator>>(std::istream& input, BigInt&)
{
	// TODO: insert return statement here
	return std::cin;
}

std::ostream& operator<<(std::ostream& output, BigInt& num)
{
	return std::cout;
}
