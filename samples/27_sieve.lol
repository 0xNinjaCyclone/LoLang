define sieve(limit) {
    composite = [false, false, false, false, false, false, false, false, false, false, false]
    p = 2

    while (p * p <= limit) {
        if (!composite[p]) {
            multiple = p * p

            while (multiple <= limit) {
                composite[multiple] = true
                multiple += p
            }
        }
        p++
    }

    return composite
}

result = sieve(10)
