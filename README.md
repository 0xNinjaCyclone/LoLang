# LoLang

LoLang is a lightweight programming language written in C, built **by hand without AI-generated implementation**, as a learning project for understanding **virtual machines, interpreters, and language runtimes**.

## Features

- Variables and assignments
- Functions with `define` and `return`
- `if` / `else`
- `while` and `for ... in`
- `stop` and `next`
- Strings, numbers, and booleans
- Arrays, objects, indexing, and member access
- Function calls
- Arithmetic, comparison, logical, bitwise, and shift operators
- Increment/decrement and compound assignments
- `need` for paths/modules
- Comments and newline/`;` statement termination
- Classes, fields, and methods.
- Interfaces and interface implementation.
- Single and multiple inheritance.
- Access modifiers: hidden, guarded, and exposed.
- Static and abstract methods.

## Why LoLang?

Building a language from scratch provides practical experience with **lexers, parsers, ASTs, interpreters, bytecode, and virtual machines**.

These concepts are valuable for advanced **malware development research**, particularly when working with:

- Custom VMs and virtualized execution
- Custom bytecode and interpreters
- Obfuscation and virtualization techniques
- Embedded scripting engines
- Custom execution environments

The project is intentionally built by hand so that every component can be understood rather than simply generated or copied. The goal is to learn **how these systems work internally**, providing a foundation for developing and researching more advanced software and malware techniques.

## Example

```lolang
define sum(a, b) {
    return a + b
}

x = sum(10, 20)

if (x > 20) {
    x += 1
}
```

## Notes

- LoLang is still under development, and its syntax and features may change.
- There is currently no runtime or virtual machine (VM).
- The project currently focuses on the lexer, parser, and AST construction.
- Some features may be incomplete or not yet functional.

LoLang is a research and educational project currently under development; it is not yet ready for executing programs or for use in real-world operational environments.