# :boar: Appa

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/d68247aae4b540d0a034e05ecd9fb7cb)](https://www.codacy.com/app/Drapegnik/Appa?utm_source=github.com&utm_medium=referral&utm_content=lybros/Appa&utm_campaign=badger)

GUI for [TheiaSfM](https://github.com/sweeneychris/TheiaSfM) multiview geometry and structure from motion library.

Implemented as a part of [uladbohdan](https://github.com/uladbohdan) and [drapegnik](https://github.com/drapegnik) coursework on [FAMCS](http://www.fpmi.bsu.by/en/main.aspx), [BSU](http://www.bsu.by/en/main.aspx) and [UIIP](http://uiip.bas-net.by/eng/), [NASB](http://nasb.gov.by/eng/).

<img src="http://res.cloudinary.com/dzsjwgjii/image/upload/v1492274096/appa.png" height="150px" >

### Supported platforms:
- [x] **macOS**
- [x] **Linux**
- [ ] **Windows**

#### macOS Sierra `10.12.4`
![](http://res.cloudinary.com/dzsjwgjii/image/upload/v1492271418/appa-mac-demo.png)

#### ubuntu `16.04`
![](http://res.cloudinary.com/dzsjwgjii/image/upload/v1492276224/appa-linux-demo.png)

### project status:
- [x] create `new project` / open `project`
- [x] extract `features`
- [x] build `model`
- [x] visualize `model`
- [ ] search `image` on `3d` `model`

for details, check the [`issues`](https://github.com/lybros/sfm/issues)

### install:
```
git clone https://github.com/lybros/Appa.git
git submodule update --init
```

### dependencies:
* [OpenImageIO](https://sites.google.com/site/openimageio/home) ~ [`v1.7.11`](https://github.com/OpenImageIO/oiio/tree/Release-1.7.11)
* [TheiaSfM](http://www.theia-sfm.org/) ~ [`v0.7`](https://github.com/sweeneychris/TheiaSfM/tree/v0.7)
* [Qt](https://www.qt.io/) ~ `v5.7`
* [QGLViewer](https://libqglviewer.com/) ~ `v2.6.4` 
* [OpenGL](https://www.opengl.org/) (*seems that you already have installed it on your computer*)
* [`cpplint`](https://github.com/cpplint/cpplint) (*optional*)
* [`Ninja`](https://ninja-build.org/manual.html) (*optional*) for build, check the [instruction](https://gist.github.com/nevkontakte/3f22937b2c3a4ff2293c)

### setup:
* set `DYLD_LIBRARY_PATH` environment variable to `path/to/openimageio/lib`
* set `OPENIMAGEIO_INCLUDE_DIR` environment variable `path/to/openimageio/include`
* set `OPENIMAGEIO_LIBRARY` environment variable to `path/to/openimageio/lib`
* set `QT_CMAKE_DIR` environment variable to `path/to/Qt/lib/cmake/`
* add `path/to/openimageio/bin` to `$PATH`
* add `path/to/theiaSfM/build/bin` to `$PATH`

### usage:
* build the code
* run `./appa` from `build/` directory
* create new `project` and run `BuildToBinary` action
* or download and open prebuilt [`quarry-project`](https://www.dropbox.com/s/0sfd02glicvwvo7/quarry.zip?dl=0) and run `VisualizeBinary` to see the result

### development:
* use `LOG(INFO)` from [glog](http://rpg.ifi.uzh.ch/docs/glog.html) instead of `std::cout`. set `GLOG_logtostderr=1` env variable to enable logging
* use `bash cpplint.sh` to check the accordance with style preferences
