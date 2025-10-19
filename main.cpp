#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

class FluxInterpreter {
private:
    Interpreter interpreter;
    
public:
    void runFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << path << std::endl;
            return;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        
        run(source);
    }
    
    void runPrompt() {
        std::cout << "Flux Programming Language v1.0" << std::endl;
        std::cout << "Type 'exit' to quit the REPL" << std::endl;
        std::cout << std::endl;
        
        std::string line;
        while (true) {
            std::cout << "flux> ";
            if (!std::getline(std::cin, line)) {
                break;
            }
            
            if (line == "exit" || line == "quit") {
                break;
            }
            
            if (line.empty()) continue;
            
            run(line);
        }
        
        std::cout << "Goodbye!" << std::endl;
    }
    
private:
    void run(const std::string& source) {
        try {
            // Tokenize
            Lexer lexer(source);
            auto tokens = lexer.tokenize();
            
            // Parse
            Parser parser(tokens);
            auto program = parser.parse();
            
            if (!program) {
                std::cerr << "Parsing failed" << std::endl;
                return;
            }
            
            // Interpret
            interpreter.interpret(*program);
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

void printUsage() {
    std::cout << "Usage: flux [script]" << std::endl;
    std::cout << "  script: Path to a .flux file to execute" << std::endl;
    std::cout << "  (no args): Start interactive REPL" << std::endl;
}

int main(int argc, char* argv[]) {
    FluxInterpreter fluxInterpreter;
    
    if (argc > 2) {
        printUsage();
        return 1;
    } else if (argc == 2) {
        // Run file
        fluxInterpreter.runFile(argv[1]);
    } else {
        // Start REPL
        fluxInterpreter.runPrompt();
    }
    
    return 0;
}