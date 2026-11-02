iface OrderRepository {
    define add(order)
    define find(order_id)
    define all()
}

class MemoryOrderRepository <- OrderRepository {
    hidden orders = {}

    exposed define add(order) {
        this.orders[order.id] = order
    }

    exposed define find(order_id) {
        if (order_id in this.orders) {
            return this.orders[order_id]
        }

        return nothing
    }

    exposed define all() {
        return this.orders
    }
}

class UnitOfWork {
    hidden repository = nothing
    hidden pending = []
    hidden committed = false

    define init(repository) {
        this.repository = repository
    }

    exposed define register_new(order) {
        this.pending.append(order)
    }

    exposed define commit() {
        for (order in this.pending) {
            this.repository.add(order)
        }

        this.pending = []
        this.committed = true

        print("Transaction committed")
    }

    exposed define rollback() {
        this.pending = []
        this.committed = false

        print("Transaction rolled back")
    }

    exposed define is_committed() {
        return this.committed
    }
}

class OrderService {
    hidden unit_of_work = nothing

    define init(unit_of_work) {
        this.unit_of_work = unit_of_work
    }

    exposed define create_order(order_id, customer, total) {
        order = {
            id: order_id,
            customer: customer,
            total: total,
            status: "created"
        }

        this.unit_of_work.register_new(order)
        this.unit_of_work.commit()

        return order
    }
}

repository = MemoryOrderRepository()
unit_of_work = UnitOfWork(repository)
service = OrderService(unit_of_work)

service.create_order("ORD-1001", "Alice", 2500)
service.create_order("ORD-1002", "Bob", 1800)

print(repository.find("ORD-1001"))
print(repository.all())

