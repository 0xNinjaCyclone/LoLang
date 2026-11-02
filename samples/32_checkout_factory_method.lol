iface PaymentProcessor {
    define pay(amount)
}

class CardProcessor <- PaymentProcessor {
    exposed define pay(amount) {
        print("Charging card: " + amount)
    }
}

class WalletProcessor <- PaymentProcessor {
    exposed define pay(amount) {
        print("Charging digital wallet: " + amount)
    }
}

class CashProcessor <- PaymentProcessor {
    exposed define pay(amount) {
        print("Payment due in cash: " + amount)
    }
}

class Checkout {
    hidden processor = nothing

    define init() {
        this.processor = this.create_processor()
    }

    abstract define create_processor()

    exposed define complete(amount) {
        print("Starting checkout")
        this.processor.pay(amount)
        print("Checkout completed")
    }
}

class CardCheckout -> Checkout {
    exposed define create_processor() {
        return CardProcessor()
    }
}

class WalletCheckout -> Checkout {
    exposed define create_processor() {
        return WalletProcessor()
    }
}

class CashCheckout -> Checkout {
    exposed define create_processor() {
        return CashProcessor()
    }
}

checkout = CardCheckout()
checkout.complete(1500)

checkout = WalletCheckout()
checkout.complete(750)

checkout = CashCheckout()
checkout.complete(300)