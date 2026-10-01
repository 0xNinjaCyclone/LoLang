define prefix_sum(a) {
    total = 0
    i = 0

    while (i < 6) {
        total += a[i]
        a[i] = total
        i++
    }

    return a
}

values = [3, 1, 4, 1, 5, 9]
result = prefix_sum(values)
