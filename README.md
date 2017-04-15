# :boar: Appa :arrow_down:
Coursework on [FAMCS](http://www.fpmi.bsu.by/en/main.aspx), [BSU](http://www.bsu.by/en/main.aspx) and [UIIP](http://uiip.bas-net.by/eng/), [NASB](http://nasb.gov.by/eng/)

<img src="http://res.cloudinary.com/dzsjwgjii/image/upload/v1492274096/appa.png" height="150px" >

Simple `ide` for building `3d` models from image data sets and visualizing it

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

for details, see [`issues`](https://github.com/lybros/sfm/issues)

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

### setup:
* set `DYLD_LIBRARY_PATH` environment variable to `path/to/openimageio/lib`
* set `OPENIMAGEIO_INCLUDE_DIR` environment variable `path/to/openimageio/include`
* set `OPENIMAGEIO_LIBRARY` environment variable to `path/to/openimageio/lib`
* set `QT_CMAKE_DIR` environment variable to `path/to/Qt/lib/cmake/`
* add `path/to/openimageio/bin` to `$PATH`
* add `path/to/theiaSfM/build/bin` to `$PATH`

### usage:
* build code
* run ./ume from `build/` directory
* create new `project` and run `BuildToBinary` action
* or open already built [`quarry-project`](https://www.dropbox.com/s/0sfd02glicvwvo7/quarry.zip?dl=0) and run `VisualizeBinary` to see result

### development:
* use `LOG(INFO)` from [glog](http://rpg.ifi.uzh.ch/docs/glog.html) instead `std::cout`. set `GLOG_logtostderr=1` env var to enable logging
* use `bash cpplint.sh` for checking style guide