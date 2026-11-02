# A legacy service with an interface we cannot change.
class LegacyShippingProvider {
    exposed define calculate_cost(weight_kg, destination_code) {
        print("Legacy provider calculating shipping")
        return weight_kg * 25
    }

    exposed define dispatch_package(package_code, destination_code) {
        print("Legacy provider dispatching " + package_code)
    }
}

# The application expects this interface.
iface ShippingService {
    define quote(package)
    define ship(package)
}

class LegacyShippingAdapter <- ShippingService {
    hidden provider = nothing

    define init(provider) {
        this.provider = provider
    }

    exposed define quote(package) {
        return this.provider.calculate_cost(
            package.weight,
            package.destination
        )
    }

    exposed define ship(package) {
        this.provider.dispatch_package(
            package.code,
            package.destination
        )
    }
}

# Facade: one simple interface for the shipping workflow.
class ShippingFacade {
    hidden shipping = nothing

    define init(shipping) {
        this.shipping = shipping
    }

    exposed define process(package) {
        cost = this.shipping.quote(package)

        print("Shipping cost: " + cost)

        this.shipping.ship(package)

        return cost
    }
}

legacy = LegacyShippingProvider()
adapter = LegacyShippingAdapter(legacy)
shipping = ShippingFacade(adapter)

package = {
    code: "PKG-1001",
    weight: 4,
    destination: "CAI"
}

shipping.process(package)

