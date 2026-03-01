#include <iostream>
#include <string>
#include <stack>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <map>

using namespace std;

// 优先级表
int prio(const string& op) {
    if (op == "u-") return 100;
    if (op == "^" || op == "**") return 90;
    if (op == "*" || op == "/" || op == "%") return 80;
    if (op == "+" || op == "-") return 70;
    return 0;
}

// 是否右结合
bool right_assoc(const string& op) {
    return op == "^" || op == "**" || op == "u-";
}

// 二元运算
double bin_op(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw runtime_error("div by zero");
        return a / b;
    }
    if (op == "%") {
        if (b == 0) throw runtime_error("mod by zero");
        return fmod(a, b);
    }
    if (op == "^" || op == "**") return pow(a, b);
    throw runtime_error("bad op");
}

// 一元运算
double un_op(double a, const string& op) {
    if (op == "u-") return -a;
    throw runtime_error("bad unary");
}

// 判断字符是否属于运算符
bool is_op_char(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

// 核心计算
double eval(const string& s) {
    stack<double> num;
    stack<string> st;
    size_t i = 0;
    bool un = true; // 期待一元运算符

    while (i < s.length()) {
        char ch = s[i];
        if (isspace(ch)) { i++; continue; }

        // 数字
        if (isdigit(ch) || ch == '.') {
            string tmp;
            while (i < s.length() && (isdigit(s[i]) || s[i] == '.'))
                tmp += s[i++];
            num.push(stod(tmp));
            un = false;
            continue;
        }

        // 左括号
        if (ch == '(') {
            st.push("(");
            i++;
            un = true;
            continue;
        }

        // 右括号
        if (ch == ')') {
            while (!st.empty() && st.top() != "(") {
                string op = st.top(); st.pop();
                if (op == "u-") {
                    double a = num.top(); num.pop();
                    num.push(un_op(a, op));
                } else {
                    double b = num.top(); num.pop();
                    double a = num.top(); num.pop();
                    num.push(bin_op(a, b, op));
                }
            }
            if (st.empty() || st.top() != "(")
                throw runtime_error("mismatched (");
            st.pop();
            i++;
            un = false;
            continue;
        }

        // 运算符
        if (is_op_char(ch)) {
            string op;
            if (ch == '*' && i+1 < s.length() && s[i+1] == '*') {
                op = "**";
                i += 2;
            } else {
                op = string(1, ch);
                i++;
            }

            // 一元负号处理
            if (op == "-" && un) {
                op = "u-";
            } else if (op == "+" && un) {
                continue; // 一元正号直接忽略
            }

            while (!st.empty() && st.top() != "(") {
                int cur = prio(op);
                int top = prio(st.top());
                bool pop = false;
                if (right_assoc(op))
                    pop = (cur > top);
                else
                    pop = (cur <= top);
                if (pop) {
                    string to = st.top(); st.pop();
                    if (to == "u-") {
                        double a = num.top(); num.pop();
                        num.push(un_op(a, to));
                    } else {
                        double b = num.top(); num.pop();
                        double a = num.top(); num.pop();
                        num.push(bin_op(a, b, to));
                    }
                } else break;
            }
            st.push(op);
            un = true;
            continue;
        }

        throw runtime_error(string("bad char: ") + ch);
    }

    while (!st.empty()) {
        string op = st.top(); st.pop();
        if (op == "(") throw runtime_error("missing )");
        if (op == "u-") {
            double a = num.top(); num.pop();
            num.push(un_op(a, op));
        } else {
            double b = num.top(); num.pop();
            double a = num.top(); num.pop();
            num.push(bin_op(a, b, op));
        }
    }

    if (num.size() != 1) throw runtime_error("invalid expr");
    return num.top();
}

int main() {
    string line;
    cout << "calc (quit to exit)\n";
    while (true) {
        cout << "> ";
        getline(cin, line);
        if (line == "quit") break;
        try {
            double r = eval(line);
            cout << r << endl;
        } catch (const exception& e) {
            cerr << "err: " << e.what() << endl;
        }
    }
    return 0;
}