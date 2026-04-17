# prol

Probable language detector in C.

## Summary

`prol` is a language detector implemented in C.

The project is split into two parts:

- `libprol`: reusable detection library
- `prol`: thin command-line interface over `libprol`

## Features

- Embedded language dataset
- 3-gram profile matching
- UTF-8 aware normalization
- No external runtime dependencies
- Small public C API
- CLI built on top of the library

## Project Layout

```text
prol.h
libprol.c
prol.c
```

- `prol.h`: public API
- `libprol.c`: detection engine
- `prol.c`: CLI wrapper

## Public API

```c
typedef struct {
    const char *code;
    double score;
} prol_result_t;

const char *prol_detect(const char *text);

int prol_detect_top(
    const char *text,
    prol_result_t *out,
    int max_results,
    double threshold
);
```

## API Notes

- `prol_detect` returns the best detected language code
- `prol_detect_top` writes ranked results into `out`
- results are ordered by descending score
- `threshold` filters weak matches
- the return value is the number of results written

## CLI Usage

### Detect one language

```bash
prol "¿Cómo estás?"
```

Expected output:

```text
es
```

### Return top results

```bash
prol -l 3 "Hello world"
```

### Filter by threshold

```bash
prol -t 0.5 "Cześć!"
```

### Read from standard input

```bash
echo "Bom dia" | prol
```

## CLI Options

- `-l <n>`: maximum number of results to print
- `-t <f>`: minimum score threshold

Default values:

- result limit: `1`
- threshold: `0.001`

## Build

### Build library and CLI

```bash
gcc -std=c99 -O3 libprol.c prol.c -I. -lm -o prol
```

### Build static library only

```bash
gcc -std=c99 -O3 -c libprol.c -I.
ar rcs libprol.a libprol.o
```

## Integration Example

```c
#include "prol.h"

int main(void) {
    const char *lang;

    lang = prol_detect("Bonjour tout le monde");
    return lang ? 0 : 1;
}
```

## Supported Languages

- English (`en`)
- Spanish (`es`)
- Portuguese (`pt`)
- French (`fr`)
- Italian (`it`)
- German (`de`)
- Dutch (`nl`)
- Swedish (`sv`)
- Danish (`da`)
- Norwegian (`no`)
- Polish (`pl`)
- Turkish (`tr`)
- Indonesian (`id`)
- Romanian (`ro`)
- Czech (`cs`)
- Hungarian (`hu`)
- Finnish (`fi`)
- Russian (`ru`)
- Ukrainian (`uk`)
- Bulgarian (`bg`)
- Greek (`el`)
- Arabic (`ar`)
- Hebrew (`he`)
- Hindi (`hi`)
- Japanese (`ja`)
- Korean (`ko`)

## Implementation Notes

- Embedded dataset
- Lazy profile training
- Internal global state
- Not thread-safe

---

**Author:** KaisarCode

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
