define selection_sort(a) {
    i = 0

    while (i < 5) {
        min = i
        j = i + 1

        while (j < 5) {
            if (a[j] < a[min]) {
                min = j
            }
            j++
        }

        tmp = a[i]
        a[i] = a[min]
        a[min] = tmp
        i++
    }

    return a
}

values = [64, 25, 12, 22, 11]
sorted = selection_sort(values)
