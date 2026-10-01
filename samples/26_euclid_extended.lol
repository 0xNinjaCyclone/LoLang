define extended_gcd(a, b) {
    old_r = a
    r = b
    old_s = 1
    s = 0

    while (r != 0) {
        q = old_r / r

        tmp = r
        r = old_r - q * r
        old_r = tmp

        tmp = s
        s = old_s - q * s
        old_s = tmp
    }

    return old_r
}

g = extended_gcd(240, 46)
