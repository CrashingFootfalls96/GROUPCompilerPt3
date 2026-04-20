#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <string>
using namespace std;

// You will need these forward references.
class Expr;
class Stmt;

// Runtime Global Variables
int pc; // program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, string> symbolvalues; // map of variables and their values CREATED IN PT3  DUMP
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer) DUMP
vector<Stmt *> insttable; // table of instructions CREATED IN PT3  DUMP
map<string, int> precMap; // CREATED IN PT3


// Runtime Global Methods
// prints vartable, instable, symboltable


// Classes Stmt and Expr
// You are allowed to add methods if needed. You should NOT need to add member variables.

class Expr {
    // expressions are evaluated!
public:
    virtual string toString() = 0;

    virtual ~Expr() {
    }
};

class StringExpr : public Expr {
public:
    virtual string eval() = 0;
};

class IntExpr : public Expr {
public:
    virtual int eval() = 0;
};

class StringConstExpr : public StringExpr {
private:
    string value;

public:
    StringConstExpr(string val) {
        value = val;
    }

    ~StringConstExpr() {
    }

    string eval() {
        return value;
    }

    string toString() {return value; }
};

class StringIDExpr : public StringExpr {
private:
    string id;  // string variable
public:
    StringIDExpr(string val) {
        id = val;
    }

    ~StringIDExpr() {
    }

    string eval() {
        // lookup symbolvalues
        return symbolvalues[id];
    }

    string toString() { return id;}
};

class StringPostFixExpr : public Expr {
private:
    vector<string> expr;
    vector<string> exprtoks;

public:
    StringPostFixExpr() {
    }

    StringPostFixExpr(string x, string t) {
        expr.push_back(x);
        exprtoks.push_back(t);
    }

    ~StringPostFixExpr() {
    }

    string eval() {
        return toString(expr);
    }

    string toString() {
        string exprConcat = "";
        for (int i = 0; i < expr.size(); i++) {
            exprConcat += expr[i] + " " + exprtoks[i];  // add out of bounds checking
        }
        return exprConcat;
    }
};

class IntConstExpr : public IntExpr {
private:
    int value;

public:
    IntConstExpr(int val) {
        value = val;
    }

    ~IntConstExpr() {
    }

    int eval() {
        return value;
    }

    string toString() { return to_string(value); }
};

class IntIDExpr : public IntExpr {
private:
    string id;

public:
    IntIDExpr(string val) {
        id = val;
    }

    ~IntIDExpr() {
    }

    int eval() {
        string valueStr = symbolvalues[id];
        return stoi(valueStr);
    }

    string toString() { return id; }
};

class IntPostFixExpr : public IntExpr {
private:
    vector<string> expr;

public:
    IntPostFixExpr() {
    }

    IntPostFixExpr(string x) { expr.push_back(x); }

    ~IntPostFixExpr() {
    }

    int eval() {
    }

    string toString() {
        string stringBuilder = "";
        for (int i = 0; i < expr.size(); i++) {
            stringBuilder += expr[i];
        }
        return stringBuilder;
    }
};

class Stmt {
    // statements are executed!
private:
    string name;

public:
    Stmt() {
    }

    virtual ~Stmt() {
    };

    virtual string toString() = 0;

    virtual void execute() = 0;
};

class AssignStmt : public Stmt {
private:
    string var;
    Expr *p_expr;

public:
    AssignStmt();

    ~AssignStmt();

    string toString();

    void execute();
};

class InputStmt : public Stmt {
private:
    string var;

public:
    InputStmt();

    ~InputStmt();

    string toString();

    void execute();
};

class StrOutStmt : public Stmt {
private:
    string value;

public:
    StrOutStmt();

    ~StrOutStmt();

    string toString();

    void execute();
};

class IntOutStmt : public Stmt {
private:
    int value;

public:
    IntOutStmt();

    ~IntOutStmt();

    string toString();

    void execute();
};

class IDOutStmt : public Stmt {
private:
    string var;

public:
    IDOutStmt();

    ~IDOutStmt();

    string toString();

    void execute();
};

class IfStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;

public:
    IfStmt();

    ~IfStmt();

    string toString();

    void execute();
};

class WhileStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;

public:
    WhileStmt();

    ~WhileStmt();

    string toString();

    void execute();
};

class GoToStmt : public Stmt {
private:
    int target;

public:
    GoToStmt();

    ~GoToStmt();

    void setTarget();

    string toString();

    void execute();
};



class Compiler {
private:
    void buildStmt();

    void buildIf();

    void buildWhile();

    void buildAssign();

    void buildInput();

    void buildOutput();

    bool isOperator(string term){
        // helper func
        if (term == "+" || term == "-" || term == "/" || term == "*" || term == "%")
            return true;
        return false;
    }

    Expr *buildExpr() {
        // ARON - shunting algorithm, uses stacks, can create local variable stack and import class
        stack<string> operStk;
        vector<string> postFix;
        for (int i = 0; i < expr.size(); i++) {
            string element = expr[i];
            if (!isOperator(element)) {
                postFix.push_back(element);
            } else {
                while (!operStk.empty() && precMap[operStk.top()] <= precMap[element]) {
                    postFix.push_back(operStk.top());
                    operStk.pop();
                }
                operStk.push(element);
            }
        }
        while (!operStk.empty()) {
            postFix.push_back(operStk.top());
            operStk.pop();
        }
        return postFix;
    }
    // headers for populate methods may not change
    void populateTokenLexemes(istream &infile);

    void populateSymbolTable(istream &infile);

public:
    Compiler() {
    }

    // headers may not change
    Compiler(istream &source, istream &symbols) {
        // build precMap - include logical, relational, arithmetic operators

        populateTokenLexemes(source);
        populateSymbolTable(symbols);
    }

    // The compile method is responsible for getting the instruction
    // table built.  It will call the appropriate build methods.
    bool compile() {
    }

    // The run method will execute the code in the instruction
    // table.
    void run() {
    }
};

void dump() {
    for (const auto & sym : symboltable) {
        cout << sym.first << " : " << sym.second << endl;
    }
    for (const auto & val : symbolvalues) {
        cout << val.first << " = " << val.second << endl;
    }
    for (int i = 0; i < insttable.size(); i++) {
        cout << i << ": " << insttable[i]->toString() << endl;
    }
}

int main() {
    ifstream source("data.txt");
    ifstream symbols("vars.txt");
    if (!source || !symbols) exit(-1);
    Compiler c(source, symbols);
    c.compile();
    // might want to call dump to check if everything built correctly
    // dump();
    c.run();
    return 0;
}
