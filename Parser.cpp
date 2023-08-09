#include "Parser.h"

map<string, TokenType> tokentypes;
map<string, int> precedence;
map<string, int> GetPrecedence(){
    map<string, int> prec = {};
    prec["="] = 1;
    prec["+"] = 2;
    prec["-"] = 2;
    prec["*"] = 3;
    prec["/"] = 3;
    return prec;
}
Parser::Parser(vector<Token> tokens){
    this->tokens = tokens;
    this->tok = tokens[0];
    tokentypes = GetTypes();
    precedence = GetPrecedence();
}
void Parser::UnexpectedToken(){
    Error::throwMessage("Unexpected token type: " + tok.type.name +  to_string(pos));
}
bool Parser::match(TokenType type){
    if (tok.type.name == type.name){
        return true;
    }
    return false;
}
bool Parser::match(TokenType type, string value){
    if (tok.type.name == type.name && tok.value == value){
        return true;
    }
    return false;
}
void Parser::require(TokenType type){
    if (match(type)){
        next();
    }
    else{
        UnexpectedToken();
    }
}
void Parser::require(TokenType type, string value){
    if (match(type, value)){
        next();
    }
    else{
        UnexpectedToken();
    }
}
bool Parser::isEOF(){
    if (pos >= tokens.size()){
        return true;
    }
    return false;
}
Token Parser::next(){
    if (!isEOF()){
        pos += 1;
        if (pos < tokens.size()){
            tok = tokens[pos];
            return tok;
        }
        return tok;
    }
    else{
        Error::throwMessage("There is no more tokens in file");
        pos += 1;
        return tokens[pos - 1];
    }
}
Token Parser::peek(){
    if (!isEOF()){
        return tokens[pos + 1];
    }
    else{
        Error::throwMessage("There is no more tokens in file");
        return tokens[pos];
    }
}
vector<shared_ptr<Node>> Parser::parseTopLevel(){
    vector<shared_ptr<Node>> prog = {};
    while (!isEOF())
    {
        prog.push_back(parsePart());
        require(tokentypes["PUNC"], ";");
    }
    return prog;
}
vector<shared_ptr<Node>> Parser::delimited(string start, string stop, string separator){
    vector<shared_ptr<Node>> exprs = {};
    bool isfirst = true;
    require(tokentypes["PUNC"], start);
    while (!match(tokentypes["PUNC"], stop))
    {
        if (isfirst){
            isfirst = false;
        }
        else{
            require(tokentypes["PUNC"], separator);
        }
        if (match(tokentypes["PUNC"], stop)){
            break;
        }
        exprs.push_back(parsePart());
    }
    return exprs;
}
shared_ptr<Node> Parser::parsePart(){
    auto token = parseAtom();
    shared_ptr<Node> expr = {};
    expr = maybeBinary(token, 0);
    return expr;
}
shared_ptr<Node> Parser::parseAtom(){
    if (match(tokentypes["NUMBER"])){
        return parseNumber();
    }
    if (match(tokentypes["VAR"])){
        if (peek().type.name == tokentypes["PUNC"].name &&  peek().value == "("){
            return parseCall();   
        }
        return parseVariable();
    }
    UnexpectedToken();
    return nullptr;
}
shared_ptr<Node> Parser::parseNumber(){
    auto num = make_shared<NumberNode>(stoi(tok.value));
    next();
    return num;
}
shared_ptr<Node> Parser::parseVariable(){
    auto var = make_shared<VariableNode>(tok.value);
    next();
    return var;
}
shared_ptr<Node> Parser::parseCall(){
    auto name = tok.value;
    next();
    auto call = make_shared<CallNode>(name, delimited("(", ")", ","));
    next();
    return call;
}
shared_ptr<Node> Parser::maybeBinary(shared_ptr<Node> left, int befprec){
    if(!match(tokentypes["OP"]) && !match(tokentypes["ASSIGN"])){
        return left;
    }
    if(precedence[tok.value] > befprec){
        Token oper = tok;
        next();
        shared_ptr<Node> lside = make_shared<BinaryOperationNode>(left, maybeBinary(parseAtom(), precedence[oper.value]), oper.value);
        return maybeBinary(lside, 0);
    }
    return left;
}