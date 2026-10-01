define binary_search(items, target) {
    left = 0
    right = 9

    while (left <= right) {
        mid = (left + right) >> 1

        if (items[mid] == target) {
            return mid
        }

        if (items[mid] < target) {
            left = mid + 1
        }
        else {
            right = mid - 1
        }
    }

    return -1
}

items = [2, 5, 8, 12, 16, 23, 38, 56, 72, 91]
index = binary_search(items, 23)
