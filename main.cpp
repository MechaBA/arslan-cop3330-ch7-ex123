/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Berkay Arslan
 */


#include "std_lib_facilities.h"

const char print = ';';
const char back = 'b';
const char num = '8';
const char c = 'c';
const char name = 'a';
const char allow = 'allow';
const string prompt = " > ";
const string result = " = ";
const string amount = "allow";
const string constant = "constant";


class tk
{
public:
    double value;
    char type;
    string name;
    tk(char ch) : type(ch), value(0) { }
    tk(char ch, double v) : type(ch), value(v) { }
    tk(char ch, string n) : type(ch), name(n) { }
};


class tk_stream
{
public:
    tk_stream();
    tk get();
    void putback(tk t);
    void ignore(char c);
private:
    bool full;
    tk buffer;
};


tk_stream::tk_stream()
:full(false), buffer(0)
{
}


void tk_stream::putback(tk t)
{
    if(full) error("Error");
    buffer = t;
    full = true;
}


tk tk_stream::get()
{
    if(full)
    {
        full = false;
        return buffer;
    }  
    char ch;
    cin >> ch;

    switch (ch)
    {
    case print:
    case back:
    case '=':
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/': 
    case '%':
    return tk(ch);
    case '.':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        cin.putback(ch);
        double v;
        cin >> v;
    return tk(num, v);
    }
    default:
        if(isalpha(ch))
        {
            string s;
            s += ch;
            while(cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s+=ch;
            cin.putback(ch);
            if(s == amount) return tk(allow);
            if(s == constant) return tk(c);
        return tk(name, s);
        }
        error("Invalid input");
    }
}


void tk_stream::ignore(char c)
{
    if(full && c == buffer.type)
    {
        full = false;
        return;
    }
    full = false;
    char ch = 0;
    while(cin>>ch)
    if(ch == c)
        return;
}

tk_stream ts;


class parameter
{
public:
    string name;
    double value;
	bool p;
    parameter(string n, double v, bool va = true) : name(n), value(v), p(va) { }
};

vector<parameter> p_table;


void set_value(string s, double d)
{
    for(int i = 0; i<p_table.size(); ++i)
        if(p_table[i].name == s)
        {
			if(p_table[i].p == false) error(s, " ");
            p_table[i].value = d;
            return;
        }
    error("Error", s);
}

double amount_value(string s)
{
    for(int i = 0; i<p_table.size(); ++i)
    if(p_table[i].name == s) return p_table[i].value;
        error("Error", s);
}


bool toAmount(string p)
{
    for(int i = 0; i<p_table.size(); ++i)
    if(p_table[i].name == p)
    return true; 
    return false;
}


double define_name(string s, double v, bool p = true)
{
    if(toAmount(s)) error(s, "Invalid input");
    p_table.push_back(parameter(s, v, p));
    return v;
}

double expression();


double sign()
{
    tk t = ts.get();
    switch(t.type)
    {
    case '(':
        {
            double d = expression();
            t = ts.get();
            if(t.type != ')') error("place: ')'");
            return d;
        }
    case num:    
    return t.value;
    case name:
		{
			tk next = ts.get();
			if(next.type == '=')
            {
				double d = expression();
				set_value(t.name, d);
				return d;
			}
			else
            {
				ts.putback(next);
				return amount_value(t.name);
			}
		}
    case '-':
    return - sign();
    case '+':
    return sign();
    default:
        error("Error");
    }
}


double exp(tk k)
{
    tk t = ts.get();
    if(t.type != name) error("Invalid input");
        string p_name = t.name;
        tk t2 = ts.get();
    if(t2.type != '=') error("Invalid input", p_name);
        double d = expression();
        define_name(p_name, d, k.type == allow);
    return d;
}


double statement()
{
    tk t = ts.get();
    switch(t.type)
    {
    case allow:
	case c:
        return exp(t.type);
    default:
        ts.putback(t);
        return expression();
    }
}


double expression()
{
    double left = term();
    tk t = ts.get();
    while(true)
    {    
        switch(t.type)
        {
        case '+':
            left += term();
            t = ts.get();
            break;
        case '-':
            left -= term();
            t = ts.get();
            break;
        default:
            ts.putback(t);
            return left;
        }
    }
}


double term()
{
    double left = sign();
    tk t = ts.get();
    while(true)
    {
        switch(t.type)
        {
        case '*':
            left *= sign();
            t = ts.get();
            break;
        case '/':
            {    
                double d = sign();
                if(d == 0) error("place: 0");
                left /= d;
                t = ts.get();
                break;
            }
        case '%':
            {    
                int i1 = narrow_cast<int>(left);
                int i2 = narrow_cast<int>(term());
                if(i2 == 0) error("place: 0");
                left = i1%i2;
                t = ts.get();
                break;
            }
        default:
            ts.putback(t);
            return left;
        }
    }
}


void errorAmount()
{ 
    ts.ignore(print);
}


void calculator()
{
    while(cin)
    try
    {
        cout << prompt;
        tk t = ts.get();
        while(t.type == print) t=ts.get();
        if(t.type == back) return;
        ts.putback(t);
        cout << result << statement() << endl;
    }
    catch(exception& e)
    {
        cerr << e.what() << endl;
        errorAmount();
    }
}


int main()
try
{
    define_name("Pi", 3.14159, false);
    define_name("Euler", 2.71828, false);
    keep_window_open();
    calculator();
    return 0;
}
catch(exception& e)
{
    cerr << e.what() << endl;
    keep_window_open(" \n");
    return 1;
}
catch(...)
{
    cerr << "Invalid\n";
    keep_window_open(" \n");
    return 2;
}