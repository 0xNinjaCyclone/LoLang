define abs(x) {
    if (x < 0) {
        return -x
    }
    return x
}

define gcd(a, b) {
    while (b != 0) {
        t = b
        b = a % b
        a = t
    }
    return abs(a)
}

define normalize(values) {
    i = 0

    while (i < 6) {
        values[i] = abs(values[i])
        i++
    }

    return values
}

values = [-12, 18, -24, 30, -42, 54]
values = normalize(values)

g = gcd(values[0], values[1])

if (g > 1) {
    status = "reducible"
}
else {
    status = "coprime"
}
