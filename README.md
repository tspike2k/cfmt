#cfmt

Proof-of-concept alternative to printf for C.

## About

This project is meant to illustrate how to go about making an alternative to printf in C. It addresses shortcomings of printf by featuring both type-safety and argument ordering through the format string. As a proof-of-concept the code isn't thoroughly tested and should not be used for production code.  It also only supports formatting signed integers. However, it should be trivial to extend.

The magic that makes this project work is C99 Variadic Macros. Variadic macros are used here to generate an array of arguments to format. The array and its length are then passed to the corresponding functions. Those functions interpret the format string, convert the specified arguments to strings, and then push the result to the destination (either a buffer or stdout). That's about it.

The general design was used rather successfully in the [Cosmic Ark clone](https://github.com/tspike2k/ark) by the author of this project.

## License

Source code is licensed under the [Boost Software License 1.0](https://www.boost.org/LICENSE_1_0.txt).
