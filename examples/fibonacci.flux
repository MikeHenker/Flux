// Fibonacci sequence in Flux

fun fibonacci(n) {
    if (n <= 1) {
        return n
    }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

print "Fibonacci sequence:"
let i = 0
while (i < 10) {
    print "fib(" + i + ") = " + fibonacci(i)
    i = i + 1
}