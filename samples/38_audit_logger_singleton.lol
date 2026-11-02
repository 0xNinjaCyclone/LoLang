class AuditLogger {
    static hidden instance = nothing
    hidden entries = []

    hidden define init() {
        print("Creating audit logger")
    }

    static exposed define get_instance() {
        if (AuditLogger.instance eq nothing) {
            AuditLogger.instance = AuditLogger()
        }

        return AuditLogger.instance
    }

    exposed define write(message) {
        this.entries.append(message)
        print("[AUDIT] " + message)
    }

    exposed define get_entries() {
        return this.entries
    }

    exposed define count() {
        return this.entries.length()
    }
}

logger_a = AuditLogger.get_instance()
logger_b = AuditLogger.get_instance()

logger_a.write("User logged in")
logger_b.write("Order #1001 created")

print("Entries: " + logger_a.count())
print(logger_a.get_entries())
