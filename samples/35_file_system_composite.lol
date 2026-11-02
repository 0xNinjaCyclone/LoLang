iface FileSystemNode {
    define get_name()
    define get_size()
    define print_tree(depth)
}

class File -> FileSystemNode {
    hidden name = ""
    hidden size = 0

    define init(name, size) {
        this.name = name
        this.size = size
    }

    exposed define get_name() {
        return this.name
    }

    exposed define get_size() {
        return this.size
    }

    exposed define print_tree(depth) {
        prefix = ""

        for (i in range(depth)) {
            prefix = prefix + "  "
        }

        print(prefix + "- " + this.name + " (" + this.size + " bytes)")
    }
}

class Directory -> FileSystemNode {
    hidden name = ""
    hidden children = []

    define init(name) {
        this.name = name
    }

    exposed define get_name() {
        return this.name
    }

    exposed define add(node) {
        this.children.append(node)
    }

    exposed define remove(node) {
        index = 0

        while (index < this.children.length()) {
            if (this.children[index] eq node) {
                this.children.remove(index)
                return
            }

            index++
        }
    }

    exposed define get_size() {
        total = 0

        for (child in this.children) {
            total = total + child.get_size()
        }

        return total
    }

    exposed define print_tree(depth) {
        prefix = ""

        for (i in range(depth)) {
            prefix = prefix + "  "
        }

        print(prefix + "+ " + this.name + "/")

        for (child in this.children) {
            child.print_tree(depth + 1)
        }
    }
}

root = Directory("project")
source = Directory("src")
tests = Directory("tests")

source.add(File("main.c", 1200))
source.add(File("parser.c", 4500))
source.add(File("lexer.c", 2800))

tests.add(File("parser_test.c", 1900))
tests.add(File("lexer_test.c", 1500))

root.add(source)
root.add(tests)
root.add(File("README.md", 800))

root.print_tree(0)
print("Total size: " + root.get_size())
