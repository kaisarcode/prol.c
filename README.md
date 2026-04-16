# prol - Probable Language Detector

A lightweight, single-file C utility for probabilistic language detection.

## Features
- **Integrated Engine**: No external maps required. Supports 26 languages out of the box.
- **N-Gram Probabilities**: Uses 3-character N-gram sliding windows for high accuracy.
- **UTF-8 Aware**: Correctly handles accent marks and non-Latin charsets (Cyrillic, Greek, Hindi, etc.).
- **Zero-Match Protection**: Discards languages with no matching n-grams to avoid false positives.

## Supported Languages
`en`, `es`, `pt`, `fr`, `it`, `de`, `nl`, `sv`, `da`, `no`, `pl`, `tr`, `id`, `ro`, `cs`, `hu`, `fi`, `ru`, `uk`, `bg`, `el`, `ar`, `he`, `hi`, `ja`, `ko`.

## Usage

### Simple detection
```bash
prol "¿Cómo estás?"
# Output: es
```

### Top Rank (Multi-result)
```bash
prol -l 3 "Hello world"
# Output: 
# en: 0.1878
# de: 0.0032
# nl: 0.0015
```

### Threshold filtering
```bash
prol -t 0.5 "Cześć!"
```

### Piped Input
```bash
echo "Bom dia" | prol
```

## Compilation

### Linux (Standard)
Build with dynamic linkage to system glibc:
```bash
gcc -O3 prol.c -o prol -lm
```

### Linux (Portable/Static)
Build a standalone binary without dependencies using `musl`:
```bash
musl-gcc -O3 -static prol.c -o prol -lm
strip prol
```

### macOS / iOS
Static linking is not supported for system libraries on Apple platforms.

**macOS:**
```bash
gcc -O3 prol.c -o prol
```

**iOS (arm64):**
```bash
xcrun -sdk iphoneos clang -O3 -arch arm64 prol.c -o prol-ios
```

### Windows
`prol` is standard C99 and can be compiled using MinGW or MSVC.

**Using MinGW (GCC):**
```bash
gcc -O3 prol.c -o prol.exe -lm
```

**Using MSVC (cl.exe):**
```bash
cl.exe /O2 prol.c /Fe:prol.exe
```

### Android
For Android, cross-compile for the `aarch64` architecture.

**Portable Static (Recommended for Termux/Shell):**
Use an `aarch64-linux-musl-gcc` cross-compiler:
```bash
aarch64-linux-musl-gcc -O3 -static prol.c -o prol-android -lm
```

**Using Android NDK:**
```bash
$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android30-clang \
    -O3 prol.c -o prol-android -lm
```

---

**Author:** KaisarCode

**Email:** <kaisar@kaisarcode.com>

**Website:** [https://kaisarcode.com](https://kaisarcode.com)

**License:** [GNU GPL v3.0](https://www.gnu.org/licenses/gpl-3.0.html)

© 2026 KaisarCode
