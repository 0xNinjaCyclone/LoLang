define find(items, target) {
    i = 0

    for (item in items) {
        if (item == target) {
            return i
        }
        i++
    }

    return -1
}

items = [7, 14, 21, 28, 35]
index = find(items, 28)
