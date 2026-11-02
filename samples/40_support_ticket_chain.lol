class SupportHandler {
    hidden next_handler = nothing

    exposed define set_next(handler) {
        this.next_handler = handler
        return handler
    }

    exposed define handle(ticket) {
        if (this.next_handler ne nothing) {
            return this.next_handler.handle(ticket)
        }

        print("No handler could resolve ticket: " + ticket.id)
        return false
    }
}

class FrontlineSupport -> SupportHandler {
    exposed define handle(ticket) {
        if (ticket.level eq "basic") {
            print("Frontline support resolved: " + ticket.id)
            return true
        }

        return base.handle(ticket)
    }
}

class TechnicalSupport -> SupportHandler {
    exposed define handle(ticket) {
        if (ticket.level eq "technical") {
            print("Technical support resolved: " + ticket.id)
            return true
        }

        return base.handle(ticket)
    }
}

class ManagerSupport -> SupportHandler {
    exposed define handle(ticket) {
        if (ticket.level eq "management") {
            print("Manager resolved: " + ticket.id)
            return true
        }

        return base.handle(ticket)
    }
}

frontline = FrontlineSupport()
technical = TechnicalSupport()
manager = ManagerSupport()

frontline.set_next(technical).set_next(manager)

for (ticket in [
    { id: "T-001", level: "basic" },
    { id: "T-002", level: "technical" },
    { id: "T-003", level: "management" },
    { id: "T-004", level: "unknown" }
]) {
    frontline.handle(ticket)
}