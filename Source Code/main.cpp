#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <stack>
#include <queue>
#include <map>
#include <cmath>
#include <iomanip>
#include <list>

// Define token types for different elements in an arithmetic expression.
enum class TokenType {
    NUMBER,
    OPERATOR,
    PARENTHESIS,
    INVALID  // Represents invalid input or tokens.
};

// Structure to hold individual tokens with their value and type.
struct Token {
    std::string value;  // The actual string value of the token.
    TokenType type;     // The type of token (e.g., NUMBER, OPERATOR).
};

// EnhancedTokenizer class is responsible for breaking up the input string into tokens.
class EnhancedTokenizer {
public:
    // Tokenize the input expression into a series of tokens.
    std::vector<Token> tokenize(const std::string& expression) {
        std::vector<Token> tokens;  // Stores the resulting tokens.
        std::istringstream iss(expression);
        char c;
        bool mayBeUnary = true;  // Flag to check if an operator can be unary.

        while (iss >> c) {  // Iterating character by character through the expression.
            if (std::isdigit(c) || c == '.') {  // Check if the character is part of a number.
                std::string number(1, c);  // Start building the number string.
                // Continue reading characters if they are digits or a decimal point.
                while (iss.peek() != EOF && (std::isdigit(iss.peek()) || iss.peek() == '.')) {
                    number += iss.get();
                }
                tokens.push_back(Token{number, TokenType::NUMBER});  // Add number token.
                mayBeUnary = false;  // After a number, an operator cannot be unary.
            } else if (isOperator(c)) {  // Check if the character is an operator.
                if (c == '-' && mayBeUnary) {  // Unary minus handling.
                    tokens.push_back(Token{"~", TokenType::OPERATOR});
                } else if (c == '+' && mayBeUnary) {  // Unary plus is skipped.
                    // Do nothing for unary plus.
                } else {
                    tokens.push_back(Token{std::string(1, c), TokenType::OPERATOR});
                }
                mayBeUnary = true;  // Reset the flag as next operator can be unary.
            } else if (c == '(' || c == ')') {  // Parentheses handling.
                tokens.push_back(Token{std::string(1, c), TokenType::PARENTHESIS});
                mayBeUnary = c == '(';  // After '(', the next operator can be unary.
            } else if (!std::isspace(c)) {  // Handling invalid characters.
                return std::vector<Token>{{std::string(1, c), TokenType::INVALID}};
            }
        }

        return tokens;  // Return the list of tokens.
    }

private:
    // Helper function to determine if a character is a valid operator.
    bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
    }
};

// ImprovedParser class transforms the sequence of tokens into a format
// suitable for evaluation (using Reverse Polish Notation).
class ImprovedParser {
public:
    // Parse the tokens into a queue representing the expression in RPN.
    std::queue<Token> parse(const std::vector<Token>& tokens) {
        std::queue<Token> outputQueue;  // Stores the tokens in RPN.
        std::stack<Token> operatorStack;  // Helps in reordering the tokens.
        // Defines operator precedence for parsing.
        std::map<std::string, int> precedence = {
            {"~", 4}, {"^", 3}, {"*", 2}, {"/", 2}, {"%", 2}, {"+", 1}, {"-", 1}
        };

        for (const auto& token : tokens) {
            if (token.type == TokenType::INVALID) {
                // Return an empty queue on encountering an invalid token.
                return std::queue<Token>();
            }

            if (token.type == TokenType::NUMBER) {
                // Directly push numbers to the output queue.
                outputQueue.push(token);
            } else if (token.type == TokenType::OPERATOR) {
                // Reorder operators based on precedence.
                while (!operatorStack.empty() &&
                       precedence[operatorStack.top().value] >= precedence[token.value]) {
                    outputQueue.push(operatorStack.top());
                    operatorStack.pop();
                }
                operatorStack.push(token);
            } else if (token.type == TokenType::PARENTHESIS) {
                // Handle parentheses in the expression.
                if (token.value == "(") {
                    operatorStack.push(token);
                } else {
                    // Pop operators until a matching '(' is found.
                    while (!operatorStack.empty() && operatorStack.top().value != "(") {
                        outputQueue.push(operatorStack.top());
                        operatorStack.pop();
                    }
                    if (!operatorStack.empty() && operatorStack.top().value == "(") {
                        operatorStack.pop();
                    } else {
                        // Unmatched parentheses detected.
                        return std::queue<Token>();
                    }
                }
            }
        }

        // Pop any remaining operators from the stack to the output queue.
        while (!operatorStack.empty()) {
            if (operatorStack.top().type == TokenType::PARENTHESIS) {
                // Unmatched parentheses detected.
                return std::queue<Token>();
            }
            outputQueue.push(operatorStack.top());
            operatorStack.pop();
        }

        return outputQueue;  // Return the parsed expression in RPN.
    }
};

// RefinedEvaluator class evaluates the expression represented in RPN.
class RefinedEvaluator {
public:
    // Evaluate the parsed expression (in RPN) and return the result.
    double evaluate(std::queue<Token> parsedExpression) {
        std::stack<double> evaluationStack;  // Stack to hold intermediate results.

        while (!parsedExpression.empty()) {
            Token token = parsedExpression.front();
            parsedExpression.pop();

            if (token.type == TokenType::NUMBER) {
                // Push numbers onto the stack.
                evaluationStack.push(std::stod(token.value));
            } else if (token.type == TokenType::OPERATOR) {
                // Evaluate the operator with operands from the stack.
                if (token.value == "~") {
                    // Unary operator handling.
                    if (evaluationStack.empty()) {
                        throw std::runtime_error("Error: Insufficient operands for unary operator");
                    }
                    double operand = evaluationStack.top(); evaluationStack.pop();
                    evaluationStack.push(-operand);
                } else {
                    // Binary operator handling.
                    if (evaluationStack.size() < 2) {
                        throw std::runtime_error("Error: Insufficient operands for operator '" + token.value + "'");
                    }
                    double right = evaluationStack.top(); evaluationStack.pop();
                    double left = evaluationStack.top(); evaluationStack.pop();

                    if ((token.value == "/" || token.value == "%") && right == 0.0) {
                        throw std::runtime_error("Error: Attempted division/modulo by zero");
                    }

                    evaluationStack.push(applyOperator(left, right, token.value));
                }
            }
        }

        if (evaluationStack.size() != 1) {
            throw std::runtime_error("Error: Invalid expression format");
        }

        return evaluationStack.top();  // Return the final result.
    }

private:
    // Apply the specified operator to the given operands.
    double applyOperator(double left, double right, const std::string& op) {
        if (op == "+") return left + right;
        if (op == "-") return left - right;
        if (op == "*") return left * right;
        if (op == "/") return left / right;  // Division by zero is checked earlier.
        if (op == "%") return std::fmod(left, right);  // Modulo by zero is checked earlier.
        if (op == "^") return std::pow(left, right);
        if (op == "~") return -left;
        throw std::runtime_error("Unknown operator");
    }
};

// CalculatorHistory class maintains a history of expressions evaluated.
class CalculatorHistory {
public:
    // Add an entry to the history with the expression and its result.
    void addEntry(const std::string& expression, const std::string& result) {
        history.emplace_back(expression, result);
    }

    // Display the history of expressions and their results.
    void showHistory() const {
        if (history.empty()) {
            std::cout << "\n--------------------------------------------------------------------------------\n";
            std::cout << "\nNo previous instances.\n";
            return;
        }
        std::cout << "\n--------------------------------------------------------------------------------\n";
        std::cout << "\nHistory:\n";
        for (const auto& entry : history) {
            std::cout << "\nExpression: " << entry.first << " | Result: " << entry.second << "\n";
        }
    }

private:
    std::list<std::pair<std::string, std::string>> history;  // List to store expression-result pairs.
};

// Function declarations for menu options.
void printMenu();
void handleExpression(CalculatorHistory& history, EnhancedTokenizer& tokenizer, ImprovedParser& parser, RefinedEvaluator& evaluator);
void showHistory(const CalculatorHistory& history);
void showUserManual();

// Function to display the main menu.
void printMenu() {
    std::cout << "\n--------------------------------------------------------------------------------\n";
    std::cout << "C++ Arithmetic Expression Evaluator\n";
    std::cout << "--------------------------------------------------------------------------------\n\n";
    std::cout << "1 - Enter Expression\n";
    std::cout << "2 - History\n";
    std::cout << "3 - User Manual\n";
    std::cout << "4 - Quit\n";
    std::cout << "\n--------------------------------------------------------------------------------\n";
    std::cout << "\nSelect an option: ";
}

// Function to handle the "Enter Expression" option.
void handleExpression(CalculatorHistory& history, EnhancedTokenizer& tokenizer, ImprovedParser& parser, RefinedEvaluator& evaluator) {
    std::string expression;
    std::cout << "\n--------------------------------------------------------------------------------\n";
    std::cout << "\nEnter an arithmetic expression: ";
    std::getline(std::cin, expression);

    auto tokens = tokenizer.tokenize(expression);
    std::string result;

    // Handling errors in tokenization, parsing, and evaluation
    if (tokens.empty() || (tokens.size() == 1 && tokens.front().type == TokenType::INVALID)) {
        result = "Error, Invalid expression";
    } else {
        auto parsedExpression = parser.parse(tokens);
        if (parsedExpression.empty()) {
            result = "Error, Invalid expression";
        } else {
            try {
                double evalResult = evaluator.evaluate(parsedExpression);
                std::ostringstream oss;
                oss << evalResult;
                result = oss.str();
            } catch (const std::runtime_error& e) {
                result = e.what();
            }
        }
    }

    std::cout << "\nResult: " << result << "\n";
    history.addEntry(expression, result);  // Add expression and result to history
}

// Function to display the history of calculations.
void showHistory(const CalculatorHistory& history) {
    history.showHistory();
}

// Function to display the user manual.
void showUserManual() {
    std::cout << "\nUser Manual:\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "\nWelcome to the C++ Arithmetic Expression Evaluator.\n";
    std::cout << "This program allows you to evaluate arithmetic expressions involving\n";
    std::cout << "basic operators such as +, -, *, /, %, and ^ (exponentiation).\n\n";
    
    std::cout << "Menu Options:\n";
    std::cout << "1 - Enter Expression: Allows you to input an arithmetic expression.\n";
    std::cout << "2 - History: Displays the history of evaluated expressions and their results.\n";
    std::cout << "3 - User Manual: Shows this user manual.\n";
    std::cout << "4 - Quit: Exits the program.\n\n";

    std::cout << "Entering Expressions:\n";
    std::cout << "Enter any arithmetic expression using numbers and operators.\n";
    std::cout << "For example: '3 + 4 * 2', '2 ^ 3', '(4 + 5) / 2'.\n";
    std::cout << "The program supports parentheses for grouping.\n\n";

    std::cout << "History:\n";
    std::cout << "After evaluating expressions, you can view their history\n";
    std::cout << "along with the results by selecting the 'History' option.\n";
    std::cout << "\n--------------------------------------------------------------------------------\n";
}

// Main program function.
int main() {
    EnhancedTokenizer tokenizer;    
    ImprovedParser parser;
    RefinedEvaluator evaluator;
    CalculatorHistory history;

    int option = 0;
    do {
        printMenu();  // Display the main menu
        std::cin >> option;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

        // Handling user menu selection
        switch (option) {
            case 1:
                handleExpression(history, tokenizer, parser, evaluator);  // Enter and evaluate an expression
                break;
            case 2:
                showHistory(history);  // Display history of expressions and results
                break;
            case 3:
                showUserManual();  // Show user manual
                break;
            case 4:
                std::cout << "\n--------------------------------------------------------------------------------\n";
                std::cout << "\nProgram has ended.\n";  // Quit the program
                std::cout << "\n--------------------------------------------------------------------------------\n";
                break;
            default:
                std::cout << "\n--------------------------------------------------------------------------------\n";
                std::cout << "\nInvalid option. Please try again.\n";  // Handle invalid menu option
                break;
        }
    } while (option != 4);

    return 0;  // End of main function
}
