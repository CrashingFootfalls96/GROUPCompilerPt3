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

class Expr;
class Stmt;

int pc;
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, string> symbolvalues;
map<string, string> symboltable;
vector<Stmt *> insttable;
map<string, int> precMap;

class Expr {
public:
    virtual string toString() = 0;
    virtual ~Expr() {}
};

class StringExpr : public Expr {
public:
    virtual string eval() = 0;
};

class IntExpr : public Expr {
public:
    virtual int eval() = 0;
};

bool isOperator(string term) {
    return precMap.count(term) > 0;
}

int applyOper(int a, int b, string oper) {
    if (oper == "*")   { return a * b; }
    if (oper == "/")   { return a / b; }
    if (oper == "%")   { return a % b; }
    if (oper == "+")   { return a + b; }
    if (oper == "-")   { return a - b; }
    if (oper == "<")   { return a < b; }
    if (oper == ">")   { return a > b; }
    if (oper == "<=")  { return a <= b; }
    if (oper == ">=")  { return a >= b; }
    if (oper == "==")  { return a == b; }
    if (oper == "!=")  { return a != b; }
    if (oper == "and") { return a && b; }
    if (oper == "or")  { return a || b; }
    return 0;
}

class StringConstExpr : public StringExpr {
private:
    string value;
public:
    StringConstExpr(string val) {
        value = val;
    }
    ~StringConstExpr() {}
    string eval() {
        return value;
    }
    string toString() {
        return value;
    }
};

class StringIDExpr : public StringExpr {
private:
    string id;
public:
    StringIDExpr(string val) {
        id = val;
    }
    ~StringIDExpr() {}
    string eval() {
        return symbolvalues[id];
    }
    string toString() {
        return id;
    }
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
    StringPostFixExpr() {}
    StringPostFixExpr(string x, string t) {
        expr.push_back(x);
        exprtoks.push_back(t);
    }
    ~StringPostFixExpr() {}
    string *eval() {
        stack<string> operandStk;
        for (const string& token : expr) {
            if (!isOperator(token)) {
                if (symboltable.contains(token)) {
                    if (!symbolvalues.count(token)) {
                        return nullptr;
                    }
                    operandStk.push(symbolvalues[token]);
                } else {
                    operandStk.push(token);
                }
            } else {
                if (operandStk.size() >= 2) {
                    string rhs = operandStk.top(); operandStk.pop();
                    string lhs = operandStk.top(); operandStk.pop();
                    string result = "";
                    if (token == "+") {
                        result = lhs + rhs;
                    } else if (token == "==") {
                        if (lhs == rhs) { result = "1"; }
                        else { result = "0"; }
                    } else if (token == "!=") {
                        if (lhs != rhs) { result = "1"; }
                        else { result = "0"; }
                    } else if (token == "<") {
                        if (lhs < rhs) { result = "1"; }
                        else { result = "0"; }
                    } else if (token == ">") {
                        if (lhs > rhs) { result = "1"; }
                        else { result = "0"; }
                    } else if (token == "<=") {
                        if (lhs <= rhs) { result = "1"; }
                        else { result = "0"; }
                    } else if (token == ">=") {
                        if (lhs >= rhs) { result = "1"; }
                        else { result = "0"; }
                    }
                    operandStk.push(result);
                }
            }
        }
        if (operandStk.empty()) {
            return nullptr;
        }
        return new string(operandStk.top());
    }
    string toString() {
        string exprConcat = "";
        for (int i = 0; i < expr.size(); i++) {
            exprConcat += expr[i];
            if (i < expr.size() - 1) {
                exprConcat += " ";
            }
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
    ~IntConstExpr() {}
    int eval() {
        return value;
    }
    string toString() {
        return to_string(value);
    }
};

class IntIDExpr : public IntExpr {
private:
    string id;
public:
    IntIDExpr(string val) {
        id = val;
    }
    ~IntIDExpr() {}
    int eval() {
        string valueStr = symbolvalues[id];
        return stoi(valueStr);
    }
    string toString() {
        return id;
    }
};

class IntPostFixExpr : public IntExpr {
private:
    vector<string> expr;
public:
    void addTerm(string term) {
        expr.push_back(term);
    }
    IntPostFixExpr() {}
    IntPostFixExpr(string x) {
        expr.push_back(x);
    }
    ~IntPostFixExpr() {}
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
            if (i < expr.size() - 1) {
                stringBuilder += " ";
            }
        }
        return stringBuilder;
    }
};

class Stmt {
private:
    string name;
public:
    Stmt() {}
    virtual ~Stmt() {}
    virtual string toString() = 0;
    virtual void execute() = 0;
    void setName(string inName) {
        name = inName;
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
        } else {
            StringPostFixExpr *spf_expr = dynamic_cast<StringPostFixExpr *>(p_expr);
            if (spf_expr) {
                string *result = spf_expr->eval();
                if (result) {
                    symbolvalues[var] = *result;
                    delete result;
                }
            } else {
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
        return "input(" + var + ")";
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

class IDOutStmt : public Stmt {
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
        return "if " + p_expr->toString() + " else target: " + to_string(elsetarget);
    }
    void execute() {
        IntExpr *i_expr = dynamic_cast<IntExpr *>(p_expr);
        if (i_expr) {
            if (i_expr->eval()) {
                pc++;
            } else {
                pc = elsetarget;
            }
        } else {
            StringPostFixExpr *spf = dynamic_cast<StringPostFixExpr *>(p_expr);
            if (spf) {
                string *result = spf->eval();
                bool cond = (result && *result == "1");
                delete result;
                if (cond) {
                    pc++;
                } else {
                    pc = elsetarget;
                }
            } else {
                pc = elsetarget;
            }
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
        return "while " + p_expr->toString() + " else target: " + to_string(elsetarget);
    }
    void execute() {
        IntExpr *i_expr = dynamic_cast<IntExpr *>(p_expr);
        if (i_expr) {
            if (i_expr->eval()) {
                pc++;
            } else {
                pc = elsetarget;
            }
        } else {
            StringPostFixExpr *spf = dynamic_cast<StringPostFixExpr *>(p_expr);
            if (spf) {
                string *result = spf->eval();
                bool cond = (result && *result == "1");
                delete result;
                if (cond) {
                    pc++;
                } else {
                    pc = elsetarget;
                }
            } else {
                pc = elsetarget;
            }
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
        return "goto: " + to_string(target);
    }
    void execute() {
        pc = target;
    }
};

class Compiler {
private:
    void buildStmt() {
        if (*tokitr == "t_if") {
            return buildIf();
        }
        if (*tokitr == "t_while") {
            return buildWhile();
        }
        if (*tokitr == "t_id") {
            if (peek("s_assign")) {
                return buildAssign();
            }
        }
        if (*tokitr == "t_input") {
            return buildInput();
        }
        if (*tokitr == "t_output") {
            return buildOutput();
        }
    }

    void buildIf() {
        tokitr++, lexitr++;
        IfStmt *istmt = new IfStmt(buildExpr());
        insttable.push_back(istmt);
        tokitr++, lexitr++;
        tokitr++, lexitr++;
        while (*tokitr != "t_end" && *tokitr != "t_else") {
            buildStmt();
        }
        if (*tokitr == "t_else") {
            GoToStmt *gtstmt = new GoToStmt();
            insttable.push_back(gtstmt);
            istmt->setElseTarget(insttable.size());
            tokitr++, lexitr++;
            while (*tokitr != "t_end") {
                buildStmt();
            }
            gtstmt->setTarget(insttable.size());
        } else {
            istmt->setElseTarget(insttable.size());
        }
        tokitr++, lexitr++;
        tokitr++, lexitr++;
    }

    void buildWhile() {
        tokitr++, lexitr++;
        int whileIdx = insttable.size();
        WhileStmt *wstmt = new WhileStmt(buildExpr());
        insttable.push_back(wstmt);
        tokitr++, lexitr++;
        tokitr++, lexitr++;
        while (*tokitr != "t_end") {
            buildStmt();
        }
        GoToStmt *gtstmt = new GoToStmt();
        gtstmt->setTarget(whileIdx);
        insttable.push_back(gtstmt);
        wstmt->setElseTarget(insttable.size());
        tokitr++, lexitr++;
        tokitr++, lexitr++;
    }

    void buildAssign() {
        string id = *lexitr;
        tokitr++, lexitr++;
        AssignStmt *asstmt = new AssignStmt(id, buildExpr());
        insttable.push_back(asstmt);
        tokitr++, lexitr++;
    }

    Expr *buildExpr() {
        Expr *expr;
        stack<string> operStk;
        tokitr++, lexitr++;
        if (peek("s_semi") || peek("s_rparen")) {
            if (*tokitr == "t_number") {
                expr = new IntConstExpr(stoi(*lexitr));
            } else if (*tokitr == "t_text") {
                expr = new StringConstExpr(*lexitr);
            } else if (*tokitr == "t_id" && symboltable[*lexitr] == "t_integer") {
                expr = new IntIDExpr(*lexitr);
            } else {
                expr = new StringIDExpr(*lexitr);
            }
            tokitr++, lexitr++;
        } else {
            if (*tokitr == "t_text" || (*tokitr == "t_id" && symboltable[*lexitr] == "t_string")) {
                StringPostFixExpr *spfExpr = new StringPostFixExpr();
                while (*tokitr != "s_semi" && *tokitr != "s_rparen") {
                    if (!isOperator(*lexitr)) {
                        spfExpr->addTerm(*lexitr, *tokitr);
                        tokitr++, lexitr++;
                    } else {
                        while (!operStk.empty() && precMap[operStk.top()] <= precMap[*lexitr]) {
                            spfExpr->addTerm(operStk.top(), *tokitr);
                            operStk.pop();
                        }
                        operStk.push(*lexitr);
                        tokitr++, lexitr++;
                    }
                }
                while (!operStk.empty()) {
                    spfExpr->addTerm(operStk.top(), *tokitr);
                    operStk.pop();
                }
                return spfExpr;
            } else {
                IntPostFixExpr *ipfExpr = new IntPostFixExpr();
                while (*tokitr != "s_semi" && *tokitr != "s_rparen") {
                    if (!isOperator(*lexitr)) {
                        ipfExpr->addTerm(*lexitr);
                        tokitr++, lexitr++;
                    } else {
                        while (!operStk.empty() && (precMap[operStk.top()] <= precMap[*lexitr])) {
                            ipfExpr->addTerm(operStk.top());
                            operStk.pop();
                        }
                        operStk.push(*lexitr);
                        tokitr++, lexitr++;
                    }
                }
                while (!operStk.empty()) {
                    ipfExpr->addTerm(operStk.top());
                    operStk.pop();
                }
                return ipfExpr;
            }
        }
        return expr;
    }

    void buildInput() {
        tokitr++, lexitr++;
        tokitr++, lexitr++;
        InputStmt *input = new InputStmt(*lexitr);
        insttable.push_back(input);
        tokitr++, lexitr++;
        tokitr++, lexitr++;
    }

    void buildOutput() {
        tokitr++, lexitr++;
        tokitr++, lexitr++;
        if (*tokitr == "t_number") {
            insttable.push_back(new IntOutStmt(stoi(*lexitr)));
        } else if (*tokitr == "t_text") {
            insttable.push_back(new StrOutStmt(*lexitr));
        } else {
            insttable.push_back(new IDOutStmt(*lexitr));
        }
        tokitr++, lexitr++;
        tokitr++, lexitr++;
    }

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
    Compiler() {}

    Compiler(istream &source, istream &symbols) {
        precMap["or"]  = 5;
        precMap["and"] = 4;
        precMap["=="]  = 3;
        precMap["!="]  = 3;
        precMap["<="]  = 3;
        precMap[">="]  = 3;
        precMap[">"]   = 3;
        precMap["<"]   = 3;
        precMap["+"]   = 2;
        precMap["-"]   = 2;
        precMap["*"]   = 1;
        precMap["/"]   = 1;
        precMap["%"]   = 1;
        populateTokenLexemes(source);
        populateSymbolTable(symbols);
    }

    bool compile() {
        while (*tokitr != "t_main") {
            tokitr++, lexitr++;
        }
        tokitr++, lexitr++;
        while (tokitr != tokens.end() && *tokitr != "t_end") {
            buildStmt();
        }
        return true;
    }

    void run() {
        pc = 0;
        while (pc < insttable.size()) {
            insttable[pc]->execute();
        }
    }
};

void dump() {
    for (const auto &sym : symboltable) {
        cout << sym.first << " " << sym.second << endl;
    }
    for (const auto &val : symbolvalues) {
        cout << val.first << " = " << val.second << endl;
    }
    for (int i = 0; i < insttable.size(); i++) {
        cout << i << ": " << insttable[i]->toString() << endl;
    }
}

int main() {
    ifstream source("data.txt");
    ifstream symbols("vars.txt");
    if (!source || !symbols) {
        exit(-1);
    }
    Compiler c(source, symbols);
    c.compile();
    dump();
    c.run();
    return 0;
}
