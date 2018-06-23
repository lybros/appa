# appa

> THE PROJECT IS NO LONGER PROPERLY MAINTAINED.
> THE MOST RECENT CHANGES ARE IN 'slam' BRANCH AND CAN BE FOUND IN [THE PULL REQUEST](https://github.com/lybros/Appa/pull/61)

GUI and CLI for [TheiaSfM](https://github.com/sweeneychris/TheiaSfM) multiview geometry and structure from motion library.

Implemented as a part of [uladbohdan](https://github.com/uladbohdan) and [drapegnik](https://github.com/drapegnik) coursework and final thesis on [FAMCS](http://www.fpmi.bsu.by/en/main.aspx), [BSU](http://www.bsu.by/en/main.aspx) and [UIIP](http://uiip.bas-net.by/eng/), [NASB](http://nasb.gov.by/eng/). The papers are also presented here (in belarusian and russian languages only).


### Supported platforms are macOS and Linux

#### macOS Sierra `10.12.4`
![](http://res.cloudinary.com/dzsjwgjii/image/upload/v1501505020/appa-main.png)

#### ubuntu `16.04`
![](http://res.cloudinary.com/dzsjwgjii/image/upload/v1492276224/appa-linux-demo.png)

### Installation
```
git clone https://github.com/lybros/Appa.git
git submodule update --init
```

### Dependencies
* [OpenImageIO](https://sites.google.com/site/openimageio/home) ~ [`v1.7.11`](https://github.com/OpenImageIO/oiio/tree/Release-1.7.11)
* [TheiaSfM](http://www.theia-sfm.org/) ~ [`v0.7`](https://github.com/sweeneychris/TheiaSfM/tree/v0.7)
* [Qt](https://www.qt.io/) ~ `v5.7`
* [QGLViewer](https://libqglviewer.com/) ~ `v2.6.4`
* [OpenGL](https://www.opengl.org/) (*seems that you already have it installed on your computer*)
* [`cpplint`](https://github.com/cpplint/cpplint) (*optional*)
* [`Ninja`](https://ninja-build.org/manual.html) (*optional*) for build, check the [instruction](https://gist.github.com/nevkontakte/3f22937b2c3a4ff2293c)

### Setup
* set `DYLD_LIBRARY_PATH` environment variable to `path/to/openimageio/lib`
* set `OPENIMAGEIO_INCLUDE_DIR` environment variable `path/to/openimageio/include`
* set `OPENIMAGEIO_LIBRARY` environment variable to `path/to/openimageio/lib`
* set `QT_CMAKE_DIR` environment variable to `path/to/Qt/lib/cmake/`
* add `path/to/openimageio/bin` to `$PATH`
* add `path/to/theiaSfM/build/bin` to `$PATH`

### Development
* use `LOG(INFO)` from [glog](http://rpg.ifi.uzh.ch/docs/glog.html) instead of `std::cout`. set `GLOG_logtostderr=1` env variable to enable logging
* use `bash cpplint.sh` to check the accordance with style preferences
