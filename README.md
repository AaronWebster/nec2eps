# nec2eps
Renders NEC files as EPS.

Simple C++ program to render NEC
[https://en.wikipedia.org/wiki/Numerical_Electromagnetics_Code](Numerical Electromagnetics Code) files as
encapsulated PostScript.

```
Usage: nec2eps [INPUT] [OUTPUT] [--scale] [--xy | --yz || --xz]

  Positional Arguments:
    INPUT     NEC input.  If unspecified, reads from standard input.
    OUTPUT    EPS output.  If unspecified, writes to standard output.

  Flags:
    --scale   Output scale.
    --xy      Output an x-y projection.
    --yz      Output an y-z projection.
    --xz      Output an x-z projection.
```
