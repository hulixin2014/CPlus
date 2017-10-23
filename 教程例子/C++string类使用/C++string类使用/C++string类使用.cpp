// C++string类使用.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<iostream>
#include<string>

using namespace std;


int main()
{
	string str1;               //生成空字符串
	string str2("1234");       //生成"1234"的复制品
	string str3("12345", 1, 3);//结果为"234"
	string str4("123456", 5);  //结果为"12345"
	string str5(5, '1');       //结果为"11111"
	string str6(str2, 2);      //结果为"34"
	cout << "str1=" << str1 << endl;
	cout << "str2=" << str2 << endl;
	cout << "str3=" << str3 << endl;
	cout << "str4=" << str4 << endl;
	cout << "str5=" << str5 << endl;
	cout << "str6=" << str6 << endl << endl;

	//大小和容量
	string s("1234567");
	cout << "size=" << s.size() << endl;
	cout << "length=" << s.length() << endl;
	cout << "max_size=" << s.max_size() << endl;
	cout << "capacity=" << s.capacity() << endl << endl;

	//字符串比较
	string A("aBcdef");
	string B("AbcdEf");
	string C("123456");
	string D("123dfg");
	int m = A.compare(B);
	int n = A.compare(1, 5, B);
	int p = A.compare(1, 5, B, 4, 2);
	int q = C.compare(0, 3, D, 0, 3);
	cout << "m=" << m <<",";
	cout << "n=" << n << ",";
	cout << "p=" << p << ",";
	cout << "q=" << q << endl <<endl;

	//内容修改
	string str7("123456");
	string str;
	str.assign(str7);
	cout << str << endl;
	str.assign(str7, 3, 3);
	cout << str << endl;
	str.assign(str7, 2, str7.npos);
	cout << str << endl;
	str.assign(5, 'X');
	cout << str << endl;
	string::iterator itB = str7.begin();
	string::iterator itE = str7.end();
	str.assign(itB, --itE);
	cout << str << endl << endl;

	str[0] = 'a';
	cout << str << endl;

	str = "1234567";
	str.erase(1, 2);
	cout << str << endl; //out:14567
	str.erase(1);
	cout << str << endl;//out:1

	str.swap(str7);
	cout << str << "  " << str7 << endl;

	str = "01234567";
	str.insert(1, "abc");
	cout << str << endl;//out:0abc1234567
	str.insert(1, "edfg", 3);
	cout << str << endl;//out:0edfabc123467
	str = "01234";
	str.insert(1, "abcde", 2, 3);
	cout << str << endl << endl;//out:0cde1234

	//查找和搜索
	str = "abc123abcabc";
	cout << str.find("abc") << endl;
	cout << str.rfind("abc") << endl;
	cout << str.find_first_of("abc",1) << endl;
	cout << str.find_last_of("abc",6) << endl;
	cout << str.find_first_not_of("abc") << endl;
	cout << str.find_last_not_of("abc") << endl;

    return 0;
}

