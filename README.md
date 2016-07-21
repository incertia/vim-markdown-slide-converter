# vim-markdown-slide-converter

This little program helps convert a markdown-styled presentation into a slide format that can be used by [presenting.vim](https://github.com/sotte/presenting.vim).

Prefix each line with `##>`, where `##` represents the frame number for the current slide. For example, the following

```
00> # stuff
00> * blah
01> * blargh
```

would generate two slides, the first containing the first two lines, and the second containing all three lines.

The multislide version is placed in `basename_final.md` and the single slide (essentially just disregards the frame number and lumps them all together) version is placed in `basename_clean.md`.
