# Flux Programming Language

Flux is a modern, expressive programming language built in C++. It features clean syntax, dynamic typing, first-class functions, and powerful control flow constructs.

## Features

- **Dynamic Typing**: Variables can hold numbers, strings, booleans, or functions
- **First-Class Functions**: Functions are values that can be passed around and called
- **Lexical Scoping**: Variables follow lexical scoping rules with proper closure support
- **Control Flow**: if/else statements, while loops, and function calls
- **Built-in Functions**: Mathematical operations, timing functions, and more
- **Interactive REPL**: Test code interactively or run script files

## Language Syntax

### Variables
```flux
let name = "Alice"
let age = 25
let isStudent = true
let value = nil
```

### Functions
```flux
fun greet(name) {
    return "Hello, " + name + "!"
}

fun factorial(n) {
    if (n <= 1) {
        return 1
    }
    return n * factorial(n - 1)
}
```

### Control Flow
```flux
// If statements
if (age >= 18) {
    print "You are an adult"
} else {
    print "You are a minor"
}

// While loops
let i = 0
while (i < 5) {
    print "Count: " + i
    i = i + 1
}
```

### Expressions and Operators
```flux
// Arithmetic
let result = 10 + 5 * 2 - 3  // 22

// Comparison
let isEqual = (5 == 5)       // true
let isGreater = (10 > 5)     // true

// Logical
let both = true and false    // false
let either = true or false   // true
let opposite = not true      // false
```

## Built-in Functions

- `print(value)` - Print a value to the console
- `clock()` - Get current time in seconds
- `sqrt(number)` - Calculate square root
- `abs(number)` - Get absolute value


### Prerequisites
- C++17 compatible compiler (g++, clang++, or MSVC)
- Make (optional, for easy building)

### Build with Make
```bash
make all          # Build the interpreter
make debug        # Build with debug symbols
make clean        # Clean build artifacts
make test         # Run example programs
```

### Build on Windows
```bash
.\build.bat       # Automated build script
.\clean.bat       # Clean all build artifacts
```

### Build Manually
```bash
# Unix/Linux
g++ -std=c++17 -O2 *.cpp -o flux

# Windows (with build directory)
mkdir build
cl /EHsc /std:c++17 /Fo:build\ /Fe:flux.exe *.cpp
```

### Running Flux

**Interactive REPL:**
```bash
./flux
```

**Execute a script:**
```bash
./flux examples/hello.flux
./flux examples/fibonacci.flux
./flux examples/advanced.flux
```

## Examples

### Hello World
```flux
print "Hello, World!"

let name = "Flux"
print "Welcome to " + name
```

### Fibonacci Sequence
```flux
fun fibonacci(n) {
    if (n <= 1) {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

print fibonacci(10)  // 55
```

### Closures and Higher-Order Functions
```flux
fun makeMultiplier(factor) {
    fun multiply(x) {
        return x * factor
    }
    return multiply
}

let double = makeMultiplier(2)
print double(5)  // 10
```

## Language Grammar

```
program     → declaration* EOF

declaration → varDecl | funDecl | statement
varDecl     → "let" IDENTIFIER ( "=" expression )? ";"?
funDecl     → "fun" IDENTIFIER "(" parameters? ")" block
parameters  → IDENTIFIER ( "," IDENTIFIER )*

statement   → exprStmt | printStmt | block | ifStmt | whileStmt | returnStmt
exprStmt    → expression ";"?
printStmt   → "print" expression ";"?
block       → "{" declaration* "}"
ifStmt      → "if" "(" expression ")" statement ( "else" statement )?
whileStmt   → "while" "(" expression ")" statement
returnStmt  → "return" expression? ";"?

expression  → assignment
assignment  → IDENTIFIER "=" assignment | logic_or
logic_or    → logic_and ( "or" logic_and )*
logic_and   → equality ( "and" equality )*
equality    → comparison ( ( "!=" | "==" ) comparison )*
comparison  → term ( ( ">" | ">=" | "<" | "<=" ) term )*
term        → factor ( ( "-" | "+" ) factor )*
factor      → unary ( ( "/" | "*" | "%" ) unary )*
unary       → ( "!" | "-" | "not" ) unary | call
call        → primary ( "(" arguments? ")" )*
primary     → NUMBER | STRING | "true" | "false" | "nil" | IDENTIFIER | "(" expression ")"
```

## Error Handling

Flux provides clear error messages for:
- Lexical errors (invalid characters, unterminated strings)
- Syntax errors (missing parentheses, invalid expressions)
- Runtime errors (undefined variables, type mismatches, division by zero)

## Contributing

Feel free to contribute to Flux by:
1. Adding new features or built-in functions
2. Improving error messages
3. Optimizing performance
4. Adding more examples

## License

This project is open source and available under the MIT License.
