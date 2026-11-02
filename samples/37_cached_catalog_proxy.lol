iface Catalog {
    define find_product(product_id)
}

iface Cache {
    define get(key)
    define set(key, value)
}

class MemoryCache <- Cache {
    hidden entries = {}

    exposed define get(key) {
        if (key in this.entries) {
            return this.entries[key]
        }

        return nothing
    }

    exposed define set(key, value) {
        this.entries[key] = value
    }
}

class LocalCatalog {
    hidden products = {}

    define init() {
        this.products["P-100"] = {
            name: "Keyboard",
            price: 1200
        }

        this.products["P-200"] = {
            name: "Mouse",
            price: 450
        }

        this.products["P-300"] = {
            name: "Monitor",
            price: 5200
        }
    }

    exposed define find_product(product_id) {
        print("Loading product from catalog")

        if (product_id in this.products) {
            return this.products[product_id]
        }

        return nothing
    }
}

class CatalogLogger {
    exposed define log(message) {
        print("[CATALOG] " + message)
    }
}

class CachedCatalog -> LocalCatalog & CatalogLogger <- Catalog {
    hidden cache = nothing

    define init(cache) {
        base.init()
        this.cache = cache
    }

    exposed define find_product(product_id) {
        cached = this.cache.get(product_id)

        if (cached ne nothing) {
            this.log("Cache hit: " + product_id)
            return cached
        }

        this.log("Cache miss: " + product_id)

        product = base.find_product(product_id)

        if (product ne nothing) {
            this.cache.set(product_id, product)
        }

        return product
    }
}

cache = MemoryCache()
catalog = CachedCatalog(cache)

first = catalog.find_product("P-100")
second = catalog.find_product("P-100")
third = catalog.find_product("P-200")

print(first)
print(second)
print(third)