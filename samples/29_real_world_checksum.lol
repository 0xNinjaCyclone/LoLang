define checksum(data) {
    hash = 0
    i = 0

    while (i < 8) {
        hash = ((hash << 5) - hash) + data[i]
        hash &= 4294967295
        i++
    }

    return hash
}

data = [84, 104, 101, 32, 81, 117, 105, 99]
result = checksum(data)
