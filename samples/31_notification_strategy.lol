iface NotificationChannel {
    define send(recipient, message)
}

class EmailChannel <- NotificationChannel {
    exposed define send(recipient, message) {
        print("Sending email to " + recipient)
        print(message)
    }
}

class SMSChannel <- NotificationChannel {
    exposed define send(recipient, message) {
        print("Sending SMS to " + recipient)
        print(message)
    }
}

class PushChannel <- NotificationChannel {
    exposed define send(recipient, message) {
        print("Sending push notification to " + recipient)
        print(message)
    }
}

class NotificationService {
    hidden channel = nothing

    define init(channel) {
        this.channel = channel
    }

    exposed define notify(recipient, message) {
        this.channel.send(recipient, message)
    }

    exposed define set_channel(channel) {
        this.channel = channel
    }
}

email = EmailChannel()
sms = SMSChannel()
push = PushChannel()

service = NotificationService(email)
service.notify("alice@example.com", "Your order has shipped")

service.set_channel(sms)
service.notify("+201000000000", "Your package is on its way")

service.set_channel(push)
service.notify("user-42", "You received a new message")
