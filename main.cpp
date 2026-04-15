// Adam Stahly
// Aron Bartoszek
// Daniel
// Nico
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
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
map<string, string> symbolvalues; // map of variables and their values
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer)
vector<Stmt *> insttable; // table of instructions
map<string, int> precMap;


// Runtime Global Methods
void dump() {
    for (const auto & lexeme : lexemes) {
        cout << symboltable[lexeme] << " ";
    }
}
    // prints vartable, insttable, symboltable


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
    }

    ~StringConstExpr() {
    }

    string eval() {
    }

    string toString() {
    }
};

class StringIDExpr : public StringExpr {
private:
    string id;

public:
    StringIDExpr(string val) {
    }

    ~StringIDExpr() {
    }

    string eval() {
    }

    string toString() {
    }
};

class StringPostFixExpr : public Expr {
private:
    vector<string> expr;
    vector<string> exprtoks;

public:
    StringPostFixExpr() {
    }

    StringPostFixExpr(string x, string t) {
    }

    ~StringPostFixExpr() {
    }

    string eval() {
    }

    string toString() {
    }
};

class IntConstExpr : public IntExpr {
private:
    int value;

public:
    IntConstExpr(int val) {
    }

    ~IntConstExpr() {
    }

    int eval() {
    }

    string toString() {
    }
};

class IntIDExpr : public IntExpr {
private:
    string id;

public:
    IntIDExpr(string val) {
    }

    ~IntIDExpr() {
    }

    int eval() {
    }

    string toString() {
    }
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

    Expr *buildExpr();

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
