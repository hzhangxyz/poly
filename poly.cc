#include<stdio.h>
#include<stdlib.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<iostream>
#include<sstream>
#include<vector>
#include<map>
#include<cmath>
#include<cctype>
#include <exception>

double string_to_double(std::string str){
    double ans;
    std::stringstream ss;
    ss << str;
    ss >> ans;
    return ans;
}

std::string & del_space(std::string &str){
    auto it = str.begin();
    while (it!=str.end()){
        if (std::isspace(*it))
          it=str.erase(it);
        else
          it++;
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef class node{
    public:
    double expn;
    double coef;
    node(){
        this->coef=0;
        this->expn=0;
    }
    node(double x,double y){
        this->coef=x;
        this->expn=y;
    }
    node(std::string &str){
        this->coef=1;
        this->expn=0;
        try{
            if(str[0]=='-'){this->coef=-1;str=str.substr(1);}
            if(str[0]=='+'){str=str.substr(1);}
            auto x=str.find("x");
            if(x==std::string::npos)this->coef*=string_to_double(str);
            else{
                if(x!=0){
                    this->coef*=string_to_double(str.substr(0,x));
                }
                auto h=str.find("^");
                this->expn = 1;
                if(h!=std::string::npos){
                    this->expn = string_to_double(str.substr(h+1));
                }
            }
        }catch(std::exception& e){  
            std::cout << e.what() << std::endl;  
        }  
    }
} node, x;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef class poly{
    public:
    std::vector<node> data;

    // +-*

    poly &operator+=(const node &n){
        auto it=this->data.begin();
        auto end=this->data.end();
        while(1){
            if(it==end){
                this->data.push_back(n);
                break;
            }
            if(it->expn==n.expn){
                it->coef+=n.coef;
                break;
            }
            if(it->expn>n.expn){
                this->data.insert(it,n);
                break;
            }
            it++;
        }
        this->checkzero();
        return *this;
    }
    poly &operator+=(poly &p){
        for(auto &it:p.data){
            (*this)+=it;
        }
        this->checkzero();
        return *this;
    }
    poly &operator+(poly &p){
        poly* ans = new poly;
        *ans = p;
        *ans += *this;
        ans->checkzero();
        return *ans;
    }
    poly &operator+=(const double &a){
        node *t = new node(a,0);
        (*this)+=*t;
        delete t;
        this->checkzero();
        return *this;
    }
    poly &operator+(const double &a){
        poly* ans = new poly;
        *ans = *this;
        *ans += a;
        ans->checkzero();
        return *ans;
    }
    friend poly &operator+(double &a,poly &p){
        return p+a;
    }
    poly &operator+(const node &n){
        poly* ans = new poly;
        *ans = *this;
        *ans += n;
        ans->checkzero();
        return *ans;
    }
    friend poly &operator+(const node &n, poly &p){
        return p+n;
    }
    poly &operator-=(const double a){
        return (*this)+=(-a);
    }
    poly &operator-(const double a){
        return (*this)+(-a);
    }
    friend poly &operator-(const double a,poly &p){
        poly* ans = new poly;
        *ans = p;
        *ans *= -1;
        *ans += a;
        ans->checkzero();
        return *ans;
    }
    poly &operator-(poly p){
        poly* ans = new poly;
        *ans = p;
        *ans *= -1;
        *ans += *this;
        ans->checkzero();
        return *ans;
    }
    poly &operator-=(poly &p){
        poly* ans = new poly;
        *ans = *this - p;
        *this = *ans;
        delete ans;
        this->checkzero();
        return *this;
    }
    poly &operator-=(const node &n){
        return *this+=node(-n.coef,n.expn);
    }
    poly &operator-(const node &n){
        poly *ans = new poly;
        *ans = *this;
        *ans -= n;
        ans->checkzero();
        return *ans;
    }
    friend poly &operator-(const node &n, poly &p){
        poly *ans = new poly;
        *ans = p;
        *ans *= -1;
        *ans += n;
        ans -> checkzero();
        return *ans;
    }
    poly &operator*=(const double a){
        for(auto &it:this->data)
            it.coef*=a;
        this->checkzero();
        return *this;
    }
    poly &operator*(const double a){
        poly* ans = new poly;
        *ans = *this;
        *ans *= a;
        ans->checkzero();
        return *ans;
    }
    friend  poly &operator*(const double a, poly &p){
        return p*a;
    }
    poly &operator*(poly &p){
        poly* ans = new poly;
        for(auto &it1:this->data)for(auto &it2:p.data){
            node * t = new node(it1.coef*it2.coef,it1.expn+it2.expn);
            *ans += *t;
            delete t;
        }
        ans->checkzero();
        return *ans;
    }
    poly &operator*=(poly &p){
        poly* ans = new poly;
        *ans = *this * p;
        *this = *ans;
        delete ans;
        this->checkzero();
        return *this;
    }
    poly &operator*=(const node &n){
        for(auto &it:this->data){
            it.expn+=n.expn;
            it.coef*=n.coef;
        }
        this->checkzero();
        return *this;
    }
    poly &operator*(const node &n){
        poly* ans = new poly;
        *ans = *this;
        *ans *= n;
        ans->checkzero();
        return *ans;
    }
    friend poly &operator*(const node &n,poly &p){
        return p*n;
    }

    // checkzero is a special function to run after every +-*

    int checkzero(){
        auto it = this->data.begin();
        while (it!=this->data.end()){
            if (std::abs(it->coef)<0.0000001)
              it=this->data.erase(it);
            else
              it++;
        }
    }

    // calc

    double operator()(const double x){
        double ans=0;
        for(auto &it:this->data){
            ans+=it.coef*std::pow(x,it.expn);
        }
        return ans;
    }

    // io

    friend std::ostream& operator<<(std::ostream &os, poly &p){
        if(p.data.empty())os << 0;
        for(auto &it:p.data){
            double fe = p.data.begin()->expn;
            std::string begin, end;
            if(it.expn==0)
                end="";
            else if(it.expn==1)
                end=" x";
            else{
                std::stringstream ss;
                ss << "x^" << it.expn;
                ss >> end;
            }
            if(it.expn==fe){
                if (std::abs(it.coef-1)<0.0000001){
                    if(it.expn!=0)begin="";
                    else begin="1";
                }else if (std::abs(it.coef+1)<0.0000001){
                    if(it.expn!=0)begin="-";
                    else begin="-1";
                }else if (it.coef>0){
                    std::stringstream ss;
                    ss << it.coef;
                    ss >> begin;
                }else{
                    std::stringstream ss;
                    ss << it.coef;
                    ss >> begin;
                }
            }
            else{
                if (std::abs(it.coef-1)<0.0000001){
                    if(it.expn!=0)begin=" +";
                    else begin=" +1";
                }else if (std::abs(it.coef+1)<0.0000001){
                    if(it.expn!=0)begin=" -";
                    else begin=" -1";
                }else if (it.coef>0){
                    std::stringstream ss;
                    ss << " +" << it.coef;
                    ss >> begin;
                }else{
                    std::stringstream ss;
                    ss << " " << it.coef;
                    ss >> begin;
                }
            }
            os << begin << end;
        }
        os << std::endl;
        return os;
    }
    friend std::istream& operator>>(std::istream &is, poly &p){
        std::string str;
        is >> str;
        p.read(str);
        return is;
    }

    // string to poly

    int read(std::string &str){
        try{
            this->unsafe_read(str);
        }catch(std::exception& e){  
            std::cout << e.what() << std::endl;  
        }  
    }
    int unsafe_read(std::string &str){
        this->data.resize(0);
        del_space(str);
        if(!str.empty()){
            std::string temp;
            auto begin = 0;
            auto end = str.find_first_of("+-",1);
            while(end!=std::string::npos){
                std::string ss = str.substr(begin,end-begin);
                *this += node(ss);
                begin = end;
                end = str.find_first_of("+-",end+1);
            }
            std::string ss = str.substr(begin);
            *this += node(ss);
        }
        this->checkzero();
    }

    // construction

    poly (){
    }
    poly (const node &n){
        this->data.push_back(n);
    }
    poly (std::string &str){
        this->read(str);
    }
}poly;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::map<std::string,poly> L;

poly &parse(const std::string &ss){
    std::string str=ss;
    del_space(str);
    if(str.substr(1).find_first_of("*+-")==std::string::npos){ // x^
        if(L.find(str.substr(1)) != L.end()){
            if (str[0]=='+'){
                return L[str.substr(1)];
            }
            else {
                auto ans = new poly;
                *ans = L[str.substr(1)];
                *ans *= -1;
                return *ans;
            }
        }if (L.find(str) != L.end()){
            return L[str];
        }
        else{
            auto ans = new poly(node(str));
            return *ans;
        }
    }else{
        int bra = 0;
        auto ans = new poly;
        auto begin = 0;
        auto end = 1;
        auto len = str.length();
        for(;end!=len;end++){
            auto &it = str[end];
            if((it=='+')||(it=='-')){
                *ans += parse(str.substr(begin,end-begin));
                begin = end;
            }
        }
        if(begin!=0){
            *ans += parse(str.substr(begin));
            return *ans;
        }else{
            *ans += node(1,0);
            auto begin = 0;
            auto end = 0;
            auto len = str.length();
            for(;end!=len;end++){
                auto &it = str[end];
                if(it=='(')bra++;
                if(it==')')bra--;
                if((bra==0)&&(it=='*')){
                    *ans *= parse(str.substr(begin,end-begin));
                    begin = end+1;
                }
            }
            *ans *= parse(str.substr(begin));
            return *ans;
        }
    }
}

int proc(char *input){
    std::string str(input);
    del_space(str);
    if(str=="")return 0;
    if(str=="exit")return 1;
    if(str=="show"){
        for(auto &it:L)
            std::cout<<it.first<<", ";
        std::cout << std::endl;
        return 0;
    }
    auto e=str.find("=");
    auto bra=str.find("(");
    if(bra!=std::string::npos){
        auto l = parse(str.substr(0,bra));
        std::stringstream ss;
        ss << str.substr(bra+1,str.length()-bra-2);
        double ans;
        ss >> ans; 
        std::cout << l(ans) << std::endl;
        return 0;
    }else if(e==std::string::npos){
        std::cout << parse(str);
        return 0;
    }else{
        std::cout << (L[str.substr(0,e)]=parse(str.substr(e+1)));
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
    char* input, shell_prompt[100];
    for(;;) {
        snprintf(shell_prompt, sizeof(shell_prompt), ">>> ");
        input = readline(shell_prompt);
        if (!input) break;
        if (proc(input)) break;
        add_history(input);
        free(input);
    }
}
