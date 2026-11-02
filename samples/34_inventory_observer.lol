iface InventoryObserver {
    define on_stock_changed(product, old_quantity, new_quantity)
}

class EmailInventoryObserver <- InventoryObserver {
    exposed define on_stock_changed(product, old_quantity, new_quantity) {
        print("Email alert: " + product)
        print("Old quantity: " + old_quantity)
        print("New quantity: " + new_quantity)
    }
}

class AuditInventoryObserver <- InventoryObserver {
    exposed define on_stock_changed(product, old_quantity, new_quantity) {
        print("AUDIT: inventory changed")
        print(product)
        print(old_quantity + " -> " + new_quantity)
    }
}

class Inventory {
    hidden quantities = {}
    hidden observers = []

    exposed define subscribe(observer) {
        this.observers.append(observer)
    }

    exposed define unsubscribe(observer) {
        index = 0

        while (index < this.observers.length()) {
            if (this.observers[index] eq observer) {
                this.observers.remove(index)
                return
            }

            index++
        }
    }

    exposed define set_stock(product, quantity) {
        old_quantity = 0

        if (product in this.quantities) {
            old_quantity = this.quantities[product]
        }

        this.quantities[product] = quantity

        this.notify(product, old_quantity, quantity)
    }

    exposed define get_stock(product) {
        if (product in this.quantities) {
            return this.quantities[product]
        }

        return 0
    }

    hidden define notify(product, old_quantity, new_quantity) {
        for (observer in this.observers) {
            observer.on_stock_changed(
                product,
                old_quantity,
                new_quantity
            )
        }
    }
}

inventory = Inventory()

email_observer = EmailInventoryObserver()
audit_observer = AuditInventoryObserver()

inventory.subscribe(email_observer)
inventory.subscribe(audit_observer)

inventory.set_stock("keyboard", 20)
inventory.set_stock("keyboard", 5)

inventory.unsubscribe(email_observer)
inventory.set_stock("keyboard", 0)
