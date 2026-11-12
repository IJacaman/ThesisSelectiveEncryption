# Selective Colour Image Encryption Based on MSB and Sensitive Bits

C++ implementation of the selective encryption method from my master's thesis and the accompanying journal article. The method locates regions of interest with Laplacian edge detection, then encrypts only the most significant and most sensitive bit planes of those regions, using bit-level operations, pixel scrambling and cipher-block chaining.

The full-encryption baseline used for comparison is in [ThesisFullEncryption](https://github.com/IJacaman/ThesisFullEncryption). MATLAB scripts used for analysis are in [MatLab](https://github.com/IJacaman/MatLab).

**Paper:** https://doi.org/10.54963/jic.v4i2.1616

## Repository layout

| Path | Contents |
| --- | --- |
| `ImageProcessing/` | Source code of the selective encryption method |
| `cryptopp/` | Bundled copy of the [Crypto++](https://www.cryptopp.com/) library (Boost Software License 1.0) |
| `images/` | Test images used in the experiments |
| `ImageProcessing.sln` | Visual Studio solution |

## Requirements

- Windows with Visual Studio 2019 or later, "Desktop development with C++" workload <!-- TODO: confirm the version you used -->
- Crypto++ (bundled in `cryptopp/`)
<!-- TODO: list any other dependency, e.g. an image-loading library, and its version -->

## Build

1. Open `ImageProcessing.sln` in Visual Studio.
2. Build the `cryptlib` project first if the solution does not do so automatically.
3. Select the `Release | x64` configuration and build the solution.

## Run

```
x64\Release\ImageProcessing.exe <input image> <output image> <key>
```
<!-- TODO: replace with the real command-line arguments or describe where the input path and key are set in the code -->

## Reproducing the paper's results

1. Build in `Release | x64`.
2. Run the program on each test image in `images/` with the key used in the paper.
3. The program reports PSNR, MSSIM, information entropy, pixel correlation, NPCR/UACI and execution time. <!-- TODO: state where each value is printed or saved -->
4. Compare with the tables in the paper; execution times will differ with hardware.

<!-- TODO: map each paper table to the exact steps that produce it, e.g. "Table 3: run on all images in images/, read entropy from the console". -->

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

My code is released under the MIT License (see `LICENSE`). The bundled Crypto++ library keeps its own license (`cryptopp/License.txt`).
