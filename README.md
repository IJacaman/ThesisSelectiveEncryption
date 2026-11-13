# Selective Colour Image Encryption (MSB and Sensitive Bits)

C++ implementation of the selective colour-image encryption scheme from my master's thesis and the journal article below. Instead of encrypting every pixel, it finds the visually important blocks of an image and encrypts only those, to reduce computation on resource-constrained devices. The full-image variant used as a baseline is in [ThesisFullEncryption](https://github.com/IJacaman/ThesisFullEncryption); the MATLAB evaluation scripts are in [MatLab](https://github.com/IJacaman/MatLab).

**Paper:** https://doi.org/10.54963/jic.v4i2.1616

## Method

1. Split the colour image into equal-sized blocks and apply Laplacian edge detection to find the blocks with the most structure (regions of interest).
2. In each selected block, XOR the most significant bit (8th bit) of every byte.
3. Swap the 7th, 6th and 5th bits between the red, green and blue channels.
4. Scramble the pixels row-wise with the Fisher–Yates shuffle.
5. XOR each byte with the previous one plus a random byte, in a mode modelled on cipher-block chaining (CBC).

Decryption applies the inverse steps with the same key.

## Requirements

- Windows with Visual Studio 2019 or later and the "Desktop development with C++" workload
- Crypto++ (included in `cryptopp/`)

## Build

1. Open `ImageProcessing.sln` in Visual Studio.
2. Select `Release | x64` and build. Build Crypto++ first if Visual Studio reports a missing `cryptlib` library.

## Run

[FILL IN: how to start the program, e.g. `ImageProcessing.exe <input image> <key>`, or which variables in which source file set the input image and key.]

Input images are read from `images/`. [FILL IN: where encrypted and decrypted images are written.]

## Reproducing the results

The thesis and paper evaluate the scheme on standard test images (Lena, Barbara and photographs of groups of people). Encrypt each image with this program and with the full-image baseline, then run the MATLAB scripts on the outputs to compute PSNR, MSSIM, entropy, pixel correlation, NPCR, UACI and key sensitivity, and compare execution times, as in the paper's comparison tables.

## Repository layout

| Path | Contents |
|---|---|
| `ImageProcessing/` | Source code of the selective scheme |
| `cryptopp/` | Bundled Crypto++ library |
| `images/` | Test images |
| `ImageProcessing.sln` | Visual Studio solution |

## Known limitations

- The random values used during encryption come from an external pseudo-random source (thesis, Section 5.2). A cryptographically secure generator should be used in any real deployment.
- Block selection is based on edges only. In images of several people it can miss faces and other body regions (thesis, Figure 5.1); learned region-of-interest detection is proposed as future work.

## Status

This repository contains the thesis code as recovered from the last working backup after the original development machine was lost. It has not yet been rebuilt on a clean machine. If you run into a build problem, please open an issue.

## Citation

If you use this code, please cite:

> Jacaman, I., & Farajallah, M. (2025). Selective Color Image Encryption Based on MSB and Sensitive Bits. *Journal of Intelligent Communication*, 4(2), 115–140. https://doi.org/10.54963/jic.v4i2.1616

```bibtex
@article{jacaman2025selective,
  author  = {Jacaman, Issa and Farajallah, Mousa},
  title   = {Selective Color Image Encryption Based on {MSB} and Sensitive Bits},
  journal = {Journal of Intelligent Communication},
  volume  = {4},
  number  = {2},
  pages   = {115--140},
  year    = {2025},
  doi     = {10.54963/jic.v4i2.1616}
}
```

Master's thesis (Palestine Polytechnic University, 2023), supervised by Dr. Mousa Farajallah: https://scholar.ppu.edu/handle/123456789/9125

## License

MIT License (see `LICENSE`). Crypto++ keeps its own license.
