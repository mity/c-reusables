[![Build status (travis-ci.com)](https://img.shields.io/travis/mity/c-reusables/master.svg?label=linux%20build)](https://travis-ci.org/mity/c-reusables)
[![Build status (appveyor.com)](https://img.shields.io/appveyor/ci/mity/c-reusables/master.svg?label=windows%20build)](https://ci.appveyor.com/project/mity/c-reusables/branch/master)
[![Codecov](https://img.shields.io/codecov/c/github/mity/c-reusables/master.svg?label=code%20coverage)](https://codecov.io/github/mity/c-reusables)


# C Reusables Readme

Home: https://github.com/mity/c-reusables


## Introduction

This repository is collection of highly reusable C utilities. The following
rules apply:

 * Licensing: Only code under MIT or in Public Domain is accepted into the
   repository.

 * Distributing: Just take the sources of interest is enough. Copyright
   notes and licensing terms are placed within each, so no aditional
   files like `LICENSE.md` are needed.

 * Self-contained: Each module is single `*.c` source file with single `*.h`
   header (or just the header) with no other dependencies but system
   headers and standard C library. Each such module implements only tightly
   related set of functions. Each header provides reasonable documentation
   of the exposed functions and types.

 * High portability: All POSIX compatible systems and Windows are supported.
   Most other systems problably too.


## Table of Contents

### Directory `encode`

 * `encode/base64.[hc]`: Encoding and decoding Base64.

 * `encode/hex.[hc]`: Encoding and decoding of bytes into/from hexadecimal
   notation (two hexadecimal digits per byte).

### Directory `hash`

 * `hash/fnv1a.[hc]`: 32-bit and 64-bit Fowler–Noll–Vo (variant 1a) hash
   functions.

### Directory `misc`

 * `misc/cmdline.[hc]`: Lightweight command line (`argc`, `argv`) parsing.

 * `misc/defs.h`: Defining set of basic an frequently used preprocessor macros
   `MIN`, `MAX`, `CLAMP`, `ABS`, `SIZEOF_ARRAY`, `OFFSETOF`, `CONTAINEROF`
   etc.

 * `misc/malloca.h`: `MALLOCA()` and `FREEA()` macros, which are a portable
   variant of `malloca()` and `freea()` from Windows SDKs by Microsoft.
   `MALLOCA()` allocates on stack if requested size below some threashold,
   for larger allocations it uses `malloc()`.
