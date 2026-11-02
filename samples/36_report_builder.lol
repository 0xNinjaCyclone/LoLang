class Report {
    exposed title = ""
    exposed author = ""
    exposed content = ""
    exposed footer = ""

    exposed define display() {
        print("==============================")
        print(this.title)
        print("Author: " + this.author)
        print("------------------------------")
        print(this.content)
        print("------------------------------")
        print(this.footer)
        print("==============================")
    }
}

class ReportBuilder {
    hidden report = nothing

    define init() {
        this.reset()
    }

    exposed define reset() {
        this.report = Report()
    }

    exposed define set_title(title) {
        this.report.title = title
        return this
    }

    exposed define set_author(author) {
        this.report.author = author
        return this
    }

    exposed define set_content(content) {
        this.report.content = content
        return this
    }

    exposed define set_footer(footer) {
        this.report.footer = footer
        return this
    }

    exposed define build() {
        result = this.report
        this.reset()
        return result
    }
}

builder = ReportBuilder()

monthly_report = builder \
    .set_title("Monthly Sales") \
    .set_author("Sales Department") \
    .set_content("Revenue increased by 12 percent.") \
    .set_footer("Confidential") \
    .build()

monthly_report.display()

security_report = builder \
    .set_title("Security Review") \
    .set_author("Security Team") \
    .set_content("No critical vulnerabilities were found.") \
    .set_footer("Internal Use Only") \
    .build()

security_report.display()
