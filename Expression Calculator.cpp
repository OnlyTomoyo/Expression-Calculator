#include <iostream>
#include <string>
#include <stack>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <map>

using namespace std;

// 获取计算优先级
int prio(string op) {
    if(op=="u-") return 10; 
    if(op=="^"||op=="**") return 9;
    if(op=="*"||op=="/"||op=="%") return 8;
    if(op=="+"||op=="-") return 7;
    return 0;
}

//进行计算
double calculate(double a, double b,string op) {
    if (op=="+") return a+b;
    if (op=="-") return a-b;
    if (op=="*") return a*b;
    if (op== "/") {
        if (b==0) throw runtime_error("div by zero");
        return a/b;
    }
    if (op=="%") {
        if (b==0) throw runtime_error("mod by zero");
        return fmod(a, b);
    }
    if (op=="^"||op=="**") return pow(a,b);
    throw runtime_error("bad op");
}

// 判断字符是否属于运算符,避免出现非运算符
bool is_op(char c) {
    return c=='+'||c=='-'||c=='*'||c=='/'||c=='%'||c=='^';
}

// 核心计算
double solve(string s) {
    stack<double> num;
    stack<string> st;
    int i=0;
    bool un=true; // 期待一元运算符

    while(i<s.length()) {
        char ch=s[i];
        if (isspace(ch))
           i++;

        // 数字
        else if(isdigit(ch)||ch=='.') {
            string tmp;
            while (i<s.length()&&(isdigit(s[i])||s[i]=='.'))
                tmp+=s[i++];
            num.push(stod(tmp));
            un=false;
        }

        // 左括号
        else if(ch=='(') {
            st.push("(");
            i++;
            un=true;
        }

        // 右括号
        else if(ch==')') {
            while (!st.empty()&&st.top()!="(") {
                string op=st.top(); 
                st.pop();
                if (op=="u-") {
                    double a=num.top(); 
                    num.pop();
                    num.push(-a);
                } else {
                    double b=num.top(); 
                    num.pop();
                    double a=num.top(); 
                    num.pop();
                    num.push(calculate(a,b,op));
                }
            }
            if(st.empty()||st.top()!="(")
                throw runtime_error("mismatched (");
            st.pop();
            i++;
            un = false;
        }

        // 运算符
        else if (is_op(ch)) {
            string op;
            if (ch=='*'&&i+1<s.length()&&s[i+1]=='*') {
                op="**";
                i+=2;
            } else {
                op=string(1,ch);
                i++;
            }
            if (op=="-"&&un) {
                op="u-";
            }

            while (!st.empty()&&st.top()!="(") {
                int cur=prio(op);
                int top=prio(st.top());
                bool is_right=false;   //判断是否是右结合计算
                if (op=="^"||op=="**"||op=="u-")
                    is_right=(cur>top);
                else
                    is_right=(cur<=top);
                if (is_right) {
                    string to=st.top(); 
                    st.pop();
                    if (to=="u-") {
                        double a=num.top(); 
                        num.pop();
                        num.push(-a);
                    } else {
                        double b=num.top(); 
                        num.pop();
                        double a=num.top(); 
                        num.pop();
                        num.push(calculate(a,b,to));
                    }
                } 
                else break;
            }
            st.push(op);
            un=true;
        }

        throw runtime_error(string("bad char: ")+ch);
    }

    while (!st.empty()) {
        string op=st.top(); 
        st.pop();
        if (op=="(") throw runtime_error("missing )");
        if (op=="u-") {
            double a=num.top(); 
            num.pop();
            num.push(-a);
        } else {
            double b=num.top(); 
            num.pop();
            double a=num.top(); 
            num.pop();
            num.push(calculate(a,b,op));
        }
    }

    return num.top();
}

int main() {
    string line;
    cout<<"calc (quit to exit)\n";
    while(true){
        cout<<">";
        getline(cin,line);
        if (line=="quit") break;
        try {
            double r=solve(line);
            cout<<r<<endl;
        } catch(const exception& e) {
            cerr<<"err: "<<e.what()<<endl;
        }
    }
    return 0;
}
