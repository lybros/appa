# sfm
structure from motion.  :mortar_board:  course work on FAMCS, BSU and UIIP, NASB

### install:
```
git clone https://github.com/lybros/sfm.git
git submodule init
git submodule update
```

### dependencies:
* [OpenImageIO](https://sites.google.com/site/openimageio/home) ~ [`v1.7.11`](https://github.com/OpenImageIO/oiio/tree/Release-1.7.11)
* [TheiaSfM](http://www.theia-sfm.org/) ~ [`v0.7`](https://github.com/sweeneychris/TheiaSfM/tree/v0.7)
* [Qt](https://www.qt.io/) ~ `v5.7`
* [QGLViewer](https://libqglviewer.com/) ~ `v2.6.4` - seems that you need OpenGL installed on your computer too.

### setup:
* set `DYLD_LIBRARY_PATH` environment variable to `path/to/openimageio/lib`
* set `OPENIMAGEIO_INCLUDE_DIR` environment variable `path/to/openimageio/include`
* set `OPENIMAGEIO_LIBRARY` environment variable to `path/to/openimageio/lib`
* set `QT_CMAKE_DIR` environment variable to `path/to/Qt/lib/cmake/`
* add `path/to/openimageio/bin` to `$PATH`
* add `path/to/theiaSfM/build/bin` to `$PATH`

### development:
* use `bash view.sh path/to/model` or just `. view.sh` for view default model from `out/model-0.txt`
* use `LOG(INFO)` from [glog](http://rpg.ifi.uzh.ch/docs/glog.html) instead `std::cout`. set `GLOG_logtostderr=1` env var to enable logging
