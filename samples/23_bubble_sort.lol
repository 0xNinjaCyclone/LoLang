define bubble_sort(a) {
    n = 5

    while (n > 1) {
        i = 0

        while (i < n - 1) {
            if (a[i] > a[i + 1]) {
                tmp = a[i]
                a[i] = a[i + 1]
                a[i + 1] = tmp
            }
            i++
        }

        n--
    }

    return a
}

values = [5, 1, 4, 2, 8]
sorted = bubble_sort(values)
