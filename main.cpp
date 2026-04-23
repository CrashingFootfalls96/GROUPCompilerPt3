// Aron Bartoszek
// Adam Stahly
// Daniel McCarthy
// Nico Ruiz
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
map<string, string> symbolvalues; // map of variables and their values
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer)
vector<Stmt *> insttable; // table of instructions
map<string, int> precMap;


// Runtime Global Methods
// prints vartable, instable, symboltable
// dump prints vartable, instable, symboltable


// Classes Stmt and Expr
// You are allowed to add methods if needed. You should NOT need to add member variables.

class Expr {
    // expressions are evaluated!
public:
    virtual string toString() = 0;

    virtual ~Expr() {
    }

    void addTerm(const vector<string>::iterator::value_type & lexitr, const vector<string>::iterator::value_type & tokitr);
};

class StringExpr : public Expr {
public:
    virtual string eval() = 0;
};

class IntExpr : public Expr {
public:
    virtual int eval() = 0;
};

bool isOperator(string term){
    // helper func
    if (term == "+" || term == "-" || term == "/" || term == "*" || term == "%")
        return true;
    return false;
}

int applyOper(int a, int b, string oper){
    if (oper == "*") return a * b;
    if (oper == "/") return a / b;
    if (oper == "%") return a % b;
    if (oper == "+") return a + b;
    if (oper == "-") return a - b;
    if (oper == "<") return a < b;
    if (oper == ">") return a > b;
    if (oper == "<=") return a <= b;
    if (oper == ">=") return a >= b;
    if (oper == "==") return a == b;
    if (oper == "!=") return a != b;
    // process a and b according to the value of oper
    // and return result
    return 0;
}

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
    void addTerm(string term, string tok) {
        expr.push_back(term);
        exprtoks.push_back(tok);
    }

    StringPostFixExpr() {
    }

    StringPostFixExpr(string x, string t) {
        // check this
        expr.push_back(x);
        exprtoks.push_back(t);
    }

    ~StringPostFixExpr() {
    }

    string *eval() {
        stack<string> operandStk;

        for (const string& token : expr) {
            if (!isOperator(token)) {
                if (symboltable.contains(token)) {
                    operandStk.push(symbolvalues[token]);
                } else {
                    operandStk.push(token);
                }
            } else {
                if (operandStk.size() >= 2) {
                    string rhs = operandStk.top(); operandStk.pop();
                    string lhs = operandStk.top(); operandStk.pop();

                    operandStk.push(lhs + rhs);
                }
            }
        }
        string result = "";
        if (!operandStk.empty()) {
            result = operandStk.top();
        }
        return  new string(result);
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
    void addTerm(string term) {
        expr.push_back(term);
    }

    IntPostFixExpr() {
    }

    IntPostFixExpr(string x) { expr.push_back(x); }

    ~IntPostFixExpr() {
    }

    int eval() {
        stack<int> operandStk;

        for (int i = 0; i < expr.size(); i++) {
            if (!isOperator(expr[i])) {
                if (symboltable.contains(expr[i])) {
                    operandStk.push(stoi(symbolvalues[expr[i]]));
                } else {
                    operandStk.push(stoi(expr[i]));
                }
            } else {
                if (operandStk.size() >= 2) {
                    int rhs = operandStk.top(); operandStk.pop();
                    int lhs = operandStk.top(); operandStk.pop();

                    int result = applyOper(lhs, rhs, expr[i]);
                    operandStk.push(result);
                }
            }
        }
        if (operandStk.empty()) {
            return 0;
        } else {
            return operandStk.top();
        }
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
    }

    virtual string toString() = 0;

    virtual void execute() = 0;

    virtual void setName(string inName){
    }
};

class AssignStmt : public Stmt {
private:
    string var;
    Expr *p_expr;

public:
    AssignStmt() {
        setName("t_assign");
        var = "";
        p_expr = nullptr;
    }

    AssignStmt(string inVar, Expr *inExpr) {
        setName("t_assign");
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
            StringPostFixExpr *spf_expr = dynamic_cast<StringPostFixExpr *>(p_expr);
            if (spf_expr) {
                symbolvalues[var] = *(spf_expr->eval());
            }
            else {
                symbolvalues[var] = dynamic_cast<StringExpr *>(p_expr)->eval();
            }
        }
        pc++;
    }
};

class InputStmt : public Stmt {
private:
    string var;

public:
    InputStmt() {
        setName("t_input");
        var = "";
    }

    InputStmt(string inVar) {
        setName("t_input");
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
        setName("t_output");
        value = "";
    }

    StrOutStmt(string inValue) {
        setName("t_output");
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
        setName("t_output");
        value = 0;
    }

    IntOutStmt(int inValue) {
        setName("t_output");
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
        setName("t_output");
        var = "";
    }

    IDOutStmt(string inVar) {
        setName("t_output");
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
        setName("t_if");
        p_expr = nullptr;
        elsetarget = -1;
    }

    IfStmt(Expr *inExpr) {
        setName("t_if");
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
        setName("t_while");
        p_expr = nullptr;
        elsetarget = -1;
    }

    WhileStmt(Expr *inExpr) {
        setName("t_while");
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
        setName("t_goto");
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
        int ifIdx = pc;
        tokitr++, lexitr++; //move past if
        tokitr++, lexitr++; //move past lparen
        IfStmt* istmt = new IfStmt(buildExpr());
        insttable.push_back(istmt);
        tokitr++, lexitr++; //move past rparen
        tokitr++, lexitr++; //move past then
        while (*tokitr != "t_end" || *tokitr != "t_else") {
            buildStmt();
        }
        if (*tokitr == "t_else") {
            GoToStmt* gtstmt = new GoToStmt();
            insttable.push_back(gtstmt);
            istmt->setElseTarget(pc);
            tokitr++, lexitr++; // skip else
            while (*tokitr != "t_end") {
                buildStmt();
            }
            gtstmt->setTarget(insttable.size());
        } else {
            istmt->setElseTarget(insttable.size());
        }
        tokitr++, lexitr++; //skip end
        tokitr++, lexitr++; //skip if
    }

    void buildWhile() {
        tokitr++, lexitr++; //move past while
        tokitr++, lexitr++; //move past lparen
        int whileIdx = pc;
        WhileStmt* wstmt = new WhileStmt(buildExpr());
        insttable.push_back(wstmt);
        tokitr++, lexitr++; //move past rparen
        tokitr++, lexitr++; // move past loop
        while (*tokitr != "t_end") {
            buildStmt();
        }
        wstmt->setElseTarget(insttable.size());
        GoToStmt* gtstmt = new GoToStmt();
        gtstmt->setTarget(whileIdx);
        insttable.push_back(gtstmt);
        tokitr++, lexitr++; //move past end
        tokitr++, lexitr++; //move past loop
    }

    void buildAssign() {
        string id = *lexitr;
        AssignStmt* asstmt = new AssignStmt(id, buildExpr());
        tokitr++, lexitr++; //skip semi
    }

    Expr *buildExpr() {
        // ARON - shunting algorithm, uses stacks, can create local variable stack, helper methods, and import classes
        Expr *expr;
        stack<string> operStk;

        tokitr++, lexitr++;
        if (peek("s_semi") || peek("s_rparen")) {
            if (*tokitr == "t_number") {
                expr = new IntConstExpr(stoi(*lexitr));
            } else if (*tokitr == "t_text") {
                expr = new StringConstExpr(*lexitr);
            } else if (*tokitr == "t_id" && symboltable[*tokitr] == "t_integer") {
                expr = new IntIDExpr(*lexitr);
            } else {
                expr = new StringIDExpr(*lexitr);
            }
        } else {
            if (*tokitr == "t_text" || (*tokitr == "t_id" && symboltable[*tokitr] == "t_string")) {
                StringPostFixExpr *expr = new StringPostFixExpr();
                while (*tokitr != "s_semi" && *tokitr != "s_rparen") {
                    if (!isOperator(*lexitr)) {
                        expr->addTerm(*lexitr, *tokitr);
                        tokitr++, lexitr++;
                    } else {
                        while (!operStk.empty() && precMap[operStk.top()] <= precMap[*lexitr]) {
                            expr->addTerm(operStk.top(), *tokitr);
                            operStk.pop();
                            tokitr++, lexitr++;
                        }
                        operStk.push(*lexitr);
                        tokitr++, lexitr++;
                    }
                }
                while (!operStk.empty()) {
                    expr->addTerm(operStk.top(), *tokitr);
                    operStk.pop();
                }
                return expr;
            } else {
                IntPostFixExpr *expr = new IntPostFixExpr();
                while (*tokitr != "s_semi" && *tokitr != "s_rparen") {
                    if (!isOperator(*lexitr)) {
                        expr->addTerm(*lexitr);
                        tokitr++, lexitr++;
                    } else {
                        while (!operStk.empty() && (precMap[operStk.top()] <= precMap[*lexitr])) {
                            expr->addTerm(operStk.top());
                            operStk.pop();
                            tokitr++, lexitr++;
                        }
                        operStk.push(*lexitr);
                        tokitr++, lexitr++;
                    }
                }
                return expr;
            }
        }
        return expr;
    }

    void buildInput() {
        tokitr++, lexitr++; //move past input
        tokitr++, lexitr++; //move past lparen
        InputStmt* input = new InputStmt(*lexitr);
        insttable.push_back(input);
        tokitr++, lexitr++; //move past id
        tokitr++, lexitr++; //move past rparen
    }

    void buildOutput() {
        tokitr++, lexitr++; //output
        tokitr++, lexitr++; //lparen
        if (symboltable[*lexitr] == "t_integer") {
            IntOutStmt* ios = new IntOutStmt(stoi(*lexitr));
            insttable.push_back(ios);
        } else if (symboltable[*lexitr] == "t_string") {
            StrOutStmt* sos = new StrOutStmt(*lexitr);
            insttable.push_back(sos);
        }
        tokitr++, lexitr++; //var
        tokitr++, lexitr++; //rparen
    }

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
        precMap["and"] = 3;
        precMap["or"] = 3;
        precMap["<="] = 3;
        precMap[">="] = 3;
        precMap[">"] = 3;
        precMap["<"] = 3;
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
        while (*tokitr != "t_main") {
            tokitr++, lexitr++; //pass all vars before main
        }
        tokitr++, lexitr++; //pass main
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
