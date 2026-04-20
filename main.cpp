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
    }

    ~StringPostFixExpr() {
    }

    string eval() {
    }

    string toString() {
        string exprConcat = "";
        for (int i = 0; i < expr.size(); i++) {
            exprConcat += expr[i] + " " + exprtoks[i];
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
    AssignStmt() {
        var = "";
        p_expr = nullptr;
    }

    AssignStmt(string inVar, Expr *inExpr) {
        var = inVar;
        p_expr = inExpr;
    }

    ~AssignStmt() {
        delete p_expr;
    }

    string toString() {
        return var + " = " + p_expr->toString();
    }

    void execute() {
        IntExpr *i_expr = dynamic_cast<IntExpr *>(p_expr);
        if (i_expr) {
            symbolvalues[var] = to_string(i_expr->eval());
        }
        else {
            symbolvalues[var] = dynamic_cast<StringExpr *>(p_expr)->eval();
        }
        pc++;
    }
};

class InputStmt : public Stmt {
private:
    string var;

public:
    InputStmt() {
        var = "";
    }

    InputStmt(string inVar) {
        var = inVar;
    }

    ~InputStmt() {}

    string toString() {
        return "input(" + var  + ")";
    }

    void execute() {
        string input;
        cout << "input value for variable: " + var << endl;
        cin >> input;
        symbolvalues[var] = input;
        pc++;
    }
};

class StrOutStmt : public Stmt {
private:
    string value;

public:
    StrOutStmt() {
        value = "";
    }

    StrOutStmt(string inValue) {
        value = inValue;
    }

    ~StrOutStmt() {}

    string toString() {
        return "output(" + value + ")";
    }

    void execute() {
        cout << value << endl;
        pc++;
    }
};

class IntOutStmt : public Stmt {
private:
    int value;

public:
    IntOutStmt() {
        value = 0;
    }

    IntOutStmt(int inValue) {
        value = inValue;
    }

    ~IntOutStmt() {}

    string toString() {
        return "output(" + to_string(value) + ")";
    }

    void execute() {
        cout << value << endl;
        pc++;
    }
};

class IDOutStmt : public Stmt { //print id value
private:
    string var;

public:
    IDOutStmt() {
        var = "";
    }

    IDOutStmt(string inVar) {
        var = inVar;
    }

    ~IDOutStmt() {}

    string toString() {
        return "output(" + var + ")";
    }

    void execute() {
        cout << symbolvalues[var] << endl;
        pc++;
    }
};

class IfStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;

public:
    IfStmt() {
        p_expr = nullptr;
        elsetarget = -1;
    }

    IfStmt(Expr *inExpr) {
        p_expr = inExpr;
        elsetarget = -1;
    }

    ~IfStmt() {
        delete p_expr;
    }

    string toString() {
        return "if " + p_expr->toString() + "else target: " + to_string(elsetarget);
    }

    void execute() {
        IntExpr *i_expr = dynamic_cast<IntExpr *>(p_expr);
        if (i_expr && i_expr->eval()) {
            pc++;
        }
        else {
            pc = elsetarget;
        }
    }
    void setElseTarget(int inTarget) {
        elsetarget = inTarget;
    }
};

class WhileStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;

public:
    WhileStmt() {
        p_expr = nullptr;
        elsetarget = -1;
    }

    WhileStmt(Expr *inExpr) {
        p_expr = inExpr;
        elsetarget = -1;
    }

    ~WhileStmt() {
        delete p_expr;
    }

    string toString() {
        return "while " + p_expr->toString() + "else target: " + to_string(elsetarget);
    }

    void execute() {
        IntExpr *i_expr = dynamic_cast<IntExpr *>(p_expr);
        if (i_expr && i_expr->eval()) {
            pc++;
        }
        else {
            pc = elsetarget;
        }
    }

    void setElseTarget(int inTarget) {
        elsetarget = inTarget;
    }
};

class GoToStmt : public Stmt {
private:
    int target;

public:
    GoToStmt() {
        target = -1;
    }

    ~GoToStmt() {}

    void setTarget(int inTarget) {
        target = inTarget;
    }

    string toString() {
        return "goto : " + to_string(target);
    }

    void execute() {
        pc = target;
    }
};



class Compiler {
private:

    void buildStmt() {
        if (*tokitr == "t_if") {return buildIf();}
        if (*tokitr == "t_while") {return buildWhile();}
        if (*tokitr == "t_id") {
            if (peek("s_assign")){return buildAssign();}
        }
        if (*tokitr == "t_input") {return buildInput();}
        if (*tokitr == "t_output") {return buildOutput();}
    }

    void buildIf() {
        tokitr++, lexitr++; //move past if
        tokitr++, lexitr++; //move past lparen


    }

    void buildWhile();

    void buildAssign() {
        string id = *lexitr;
        tokitr++, lexitr++; //move past id
        tokitr++, lexitr++; //move past assign

    }

    void buildInput() {
        tokitr++, lexitr++; //move past input
        tokitr++, lexitr++; //move past lparen
        // InputStmt* input = new InputStmt(*lexitr);
        // insttable.push_back(input);
        tokitr++, lexitr++; //move past id
        tokitr++, lexitr++; //move past rparen
    }

    void buildOutput() {
        tokitr++, lexitr++; //output
        tokitr++, lexitr++; //lparen
        if (symboltable[*lexitr] == "t_integer") {
            // IntOutStmt* ios = new IntOutStmt(*lexitr);
            // insttable.push_back(ios);
        } else if (symboltable[*lexitr] == "t_string") {
            // StrOutStmt sos = new StrOutStmt(*lexitr);
            // insttable.push_back(sos);
        }
        tokitr++, lexitr++; //var
        tokitr++, lexitr++; //rparen
    }

    Expr *buildExpr();

    // headers for populate methods may not change
    void populateTokenLexemes(istream &infile) {
        string tok, lex, line;
        while (getline(infile, line)) {
            int spacePos = line.find(' ');
            tok = line.substr(0, spacePos);
            lex = line.substr(spacePos + 1);

            tokens.push_back(tok);
            lexemes.push_back(lex);
        }
        tokitr = tokens.begin();
        lexitr = lexemes.begin();
    }

    void populateSymbolTable(istream &infile) {
        string id, type, line;
        while (getline(infile, line)) {
            int spacePos = line.find(' ');
            id = line.substr(0, spacePos);
            type = line.substr(spacePos + 1);

            symboltable.insert(make_pair(id, type));
        }
    }
    bool peek(string token) {
        bool match = false;
        tokitr++, lexitr++;
        if (*tokitr == token) {
            match = true;
        }
        tokitr--, lexitr--;
        return match;
    }

public:
    Compiler() {
    }

    // headers may not change
    Compiler(istream &source, istream &symbols) {
        // build precMap - include logical, relational, arithmetic operators
        precMap["<"] = 3;
        precMap[">"] = 3;
        precMap["<="] = 3;
        precMap[">="] = 3;
        precMap["+"] = 2;
        precMap["-"] = 2;
        precMap["*"] = 1;
        precMap["/"] = 1;
        precMap["%"] = 1;

        populateTokenLexemes(source);
        populateSymbolTable(symbols);
    }

    // The compile method is responsible for getting the instruction
    // table built.  It will call the appropriate build methods.
    bool compile() {
        while (tokitr != tokens.end()) {
            buildStmt();
        }
        //Compiles correctly only when there are no logical errors
        return true;
    }

    // The run method will execute the code in the instruction
    // table.
    void run() {
        pc = 0;
        while (pc < insttable.size()) {
            insttable[pc]->execute();
            pc++;
        }
    }
};

void dump() {
    for (const auto & sym : symboltable) {
        cout << sym.first << " " << sym.second << endl;
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
