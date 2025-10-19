// Advanced features in Flux

// Nested functions and closures
fun makeCounter() {
    let count = 0
    
    fun increment() {
        count = count + 1
        return count
    }
    
    return increment
}

let counter = makeCounter()
print "Counter: " + counter()  // 1
print "Counter: " + counter()  // 2
print "Counter: " + counter()  // 3

// Conditional logic with complex expressions
fun isPrime(n) {
    if (n < 2) return false
    if (n == 2) return true
    if (n % 2 == 0) return false
    
    let i = 3
    while (i * i <= n) {
        if (n % i == 0) {
            return false
        }
        i = i + 2
    }
    return true
}

print "Prime numbers up to 20:"
let num = 2
while (num <= 20) {
    if (isPrime(num)) {
        print num + " is prime"
    }
    num = num + 1
}

// Using built-in functions
print "Square root of 16: " + sqrt(16)
print "Absolute value of -42: " + abs(-42)
print "Current time: " + clock() + " seconds"

// String and boolean operations
let message = "Flux is awesome!"
print message
print "Length check: " + (message == "Flux is awesome!")

// Logical operators
let a = true
let b = false
print "a and b: " + (a and b)
print "a or b: " + (a or b)
print "not a: " + (not a)