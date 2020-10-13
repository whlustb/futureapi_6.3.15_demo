#include<iostream>
#include<stack>
#include<string>
#include<vector>
#include<map>
#include<algorithm>
using namespace std;

map<const char, int> priority;//用map来储存运算符的优先级
int compare(char a, char b) {
	int cmp = 0;
	if (b == ')' && a == '(') cmp = 0;
	else if (a == '(') cmp = -1;
	else if (priority[a] < priority[b]) cmp = -1;
	else if (priority[a] >= priority[b]) cmp = 1;
	return cmp;
}
int cal(int a, int b, char op) {
	int ans;
	if (op == '+') ans = a + b;
	else if (op == '-') ans = a - b;
	else if (op == '*') ans = a * b;
	else if (op == '/') ans = a / b;
	return ans;
}
int calculator() {
	//根据优先级的关系，可以把他们的优先级定义为下面的形式
	//个别运算符的优先级是与位置前后有关系的，需要特殊处理
	priority['#'] = -1;
	priority[')'] = 0;
	priority['+'] = 1;
	priority['-'] = 1;
	priority['*'] = 2;
	priority['/'] = 2;
	priority['('] = 3;
	cout << "please input valid expression, enter to terminate..." << endl;
	char ch = getchar();
	stack<char> op;
	stack<int> nums;
	op.push('#');
	nums.push(0);
	bool flag = true;
	while (ch != '#' || op.top() != '#') {
		if (ch <= '9' && ch >= '0') {
			int number = 0;
			while (ch >= '0' && ch <= '9') {//连续出现的数字看做一个整体
				number = number * 10 + (ch - '0');
				ch = getchar();
			}
			//    cout<<"number: "<<number<<endl;
			//    cout<<"op: "<<ch<<endl;
			nums.push(number);
			flag = true;
		} else {//比较栈顶运算符和新输出运算符的优先级
			int cmp = compare(op.top(), ch);
			//cout<<"compare("<<op.top()<<","<<ch<<") = "<<cmp<<endl;
			if (cmp == -1) {//顶部优先级低时，把新的运算符压栈
				op.push(ch);
				flag = false;
				ch = getchar();
			} else if (cmp == 0) {//即栈顶和新的运算符是'('和')'，需要把'('弹出
				op.pop();
				ch = getchar();
			} else if (cmp == 1) {//栈顶运算符优先级高的时候，就要进行运算
				int num1, num2, tempans;
				char tempop;//一定要注意num的顺序，否则会导致错误的运算结果
				num2 = nums.top();
				nums.pop();
				num1 = nums.top();
				nums.pop();
				tempop = op.top();
				op.pop();
				tempans = cal(num1, num2, tempop);
				//cout<<"tempans: "<<tempans<<endl;
				nums.push(tempans);
			}
			if (ch == '-' && !flag) nums.push(0);
		}
	}
	cin.get();
	return nums.top();
}
int main() {
	int i = 10;
	while (i--) {
		cout << calculator() << endl;
	}
	return 0;
}