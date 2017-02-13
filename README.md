# sfm
structure from motion.  :mortar_board:  course work on FAMCS, BSU and UIIP, NASB

### Dependencies:
* [OpenImageIO](https://sites.google.com/site/openimageio/home) ~ [`v1.7.11`](https://github.com/OpenImageIO/oiio/tree/Release-1.7.11)
* [TheiaSfM](http://www.theia-sfm.org/) ~ [`v0.7`](https://github.com/sweeneychris/TheiaSfM/tree/v0.7)

### Setup:
* set `DYLD_LIBRARY_PATH` environment variable to `path/to/openimageio/lib`
* set `OPENIMAGEIO_INCLUDE_DIR` environment variable `path/to/openimageio/include`
* set `OPENIMAGEIO_LIBRARY` environment variable to `path/to/openimageio/lib`
* add `path/to/openimageio/bin` to `$PATH`
* add `path/to/theiaSfM/build/bin` to `$PATH`

# Development:
* use `bash view.sh path/to/model` or just `. view.sh` for view default model from `out/model-0.txt`
