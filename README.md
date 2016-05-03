OpenSSE: Open Sketch Search Engine [![Build Status](https://travis-ci.org/zddhub/opensse.svg?branch=master)](https://travis-ci.org/zddhub/opensse)
==================================

[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/zddhub/opensse?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Hello everybody!

I'm very interested in [Mathias Eitz's works](http://cybertron.cg.tu-berlin.de/eitz/projects/sbsr/) on SIGGRAPH 2012, so I developed OpenSSE, Just for fun.

This is an open source sketch search engine for 3D object retrieval based on sketch image as input. In fact, it can retrieve more, such as images, videos, point cloud models and volume models, you just need get their line drawing images than I know you can.

In order to clearly show how to work, I used single thread and ascii encoding for file storage.

Zhang Dongdong

Demo - SketchSearchDemo
=======================

This demo deals with mesh models. See more search results, click [demo videos](http://opensse.com).

![Sketch search demo](http://img.blog.csdn.net/20140515204902125 "Sketch search demo")

Demo - SketchRecognize
======================

[Try via web](http://online.opensse.com)

See more search results, click [demo videos](http://sr.opensse.com).

![demo video](http://img.blog.csdn.net/20140520195606468 "Sketch recognize demo")

How to use
==========

### Use OpenSSE library

You just need include one line in your `.pro` file.
```cpp
include(../../opensse.pri)
```

### Use tools

There are a series of tools for you under `opensse/tools/`. When compiled, those tools are under `build-xxx-Release/bin`.

### Use gui

This is gui demo for you under `opensse/gui`.

How to train data
=================

### Database

Database in [my demo](http://opensse.com) comes from [SHREC 2012](http://www.itl.nist.gov/iad/vug/sharp/contest/2012/SBR/data.html), download [3D target dataset](http://www.itl.nist.gov/iad/vug/sharp/contest/2012/SBR/Watertight_dataset.zip)(~112MB). We use models under directory `Watertight_dataset/Extended`.
```shell
$ mv /Users/zdd/Download/Watertight_dataset/Extended /Users/zdd/Database/SHREC12/
```

### Get line drawing views

Using my [another project](https://github.com/zddhub/trianglemesh), you will get 102 views for each model.

### Generate vocabulary

- Step 1: Generate line draing images filelist
```shell
$ ./generate_filelist -d /Users/zdd/Database/SHREC12/ -f "*.jpg" -o /Users/zdd/Database/SHREC12/2012_filelist
```
- Step 2: Extract descriptors
```shell
$ ./extract_descriptors -d /Users/zdd/Database/SHREC12/ -f /Users/zdd/Database/SHREC12/2012_filelist -o /Users/zdd/Database/SHREC12/2012_
```
you will gain two files: `2012_features` and `2012_keypoints`.
- Step 3: Generate vocabulary (optional)
```shell
$ ./generate_vocabulary -f /Users/zdd/Database/SHREC12/2012_features -n 1000 -o /Users/zdd/Database/SHREC12/vocabulary
```
It will take you some times, but only need run once. or use my vocabulary under `../opensse/data/`.

### Create inverted index file 

- Step 4: Quantize feature
```shell
$ ./quantize -v /Users/zdd/Database/SHREC12/vocabulary -f /Users/zdd/Database/SHREC12/2012_features -o /Users/zdd/Database/SHREC12/2012_samples
```

When dealing with large scale database, we can directly generate samples using extract_quantize tool to reduce disk usage without feature files.
```shell
$ ./extract_and_quantize -d /Users/zdd/Database/SHREC12/ -f /Users/zdd/Database/SHREC12/2012_filelist -v /Users/zdd/Database/SHREC12/2012_filelist -v /Users/zdd/Database/SHREC12/vocabulary -o /Users/zdd/Database/SHREC12/2012_samples
```

- Step 5: Create inverted index file
```shell
$ ./create_index -s /Users/zdd/Database/SHREC12/2012_samples -o /Users/zdd/Database/SHREC12/2012_index_file
```
Now all data are ready.

Test OpenSSE
============

### Command line
You can test opensse through command line
```shell
$ ./sketch_search -i /Users/zdd/Database/SHREC12/2012_index_file -v /Users/zdd/Database/SHREC12/vocabulary -d /Users/zdd/Database/SHREC12/ -f /Users/zdd/Database/SHREC12/2012_filelist -n 10 -o output
```
You will get a interactive interface, like:
```shell
>> open sketch search :
>> input absolute path, like "/Users/zdd/zddhub.png"
>> input q exit
>> good luck!
>> /Users/zdd/Database/SHREC12/Extended/D00057view/1.jpg
0.985509 /Users/zdd/Database/SHREC12/Extended/D00057view/1.jpg
0.953196 /Users/zdd/Database/SHREC12/Extended/D00050view/1.jpg
0.938705 /Users/zdd/Database/SHREC12/Extended/D00041view/1.jpg
0.856004 /Users/zdd/Database/SHREC12/Extended/D00050view/2.jpg
0.852683 /Users/zdd/Database/SHREC12/Extended/D00057view/2.jpg
0.849087 /Users/zdd/Database/SHREC12/Extended/D00041view/2.jpg
0.823276 /Users/zdd/Database/SHREC12/Extended/D00041view/0.jpg
0.821133 /Users/zdd/Database/SHREC12/Extended/D00050view/10.jpg
0.811917 /Users/zdd/Database/SHREC12/Extended/D00057view/10.jpg
0.810096 /Users/zdd/Database/SHREC12/Extended/D00050view/0.jpg
>> q
```

### Gui Demo - SketchSearchDemo
You must config `params.json` file using your data path, like:
```json
{
    "searcher":
    {
        "indexfile": "/Users/zdd/Database/SHREC12/2012_index_file",
        "filelist": "/Users/zdd/Database/SHREC12/2012_filelist",
        "vocabulary": "/Users/zdd/Database/SHREC12/vocabulary",
        "rootdir": "/Users/zdd/Database/SHREC12",
        "results_num": "12",
        "views_num": "102"
    }
}
```
You'd better to use the absolute path. And then, set `params.json` in `../opensse/gui/SketchSearchDemo/mainwindow.cpp`:
```cpp
boost::property_tree::read_json("/Users/zdd/Database/SHREC12/params.json", params); 
```

Compiled and You will get [my demo](http://opensse.com). Good luck!

Notice: To make sure line drawing images correspond to 3d models, please copy `../opensse/data/view/` to you execute file path.


How to compile
==============

OpenSSE uses [Qt 5.1+](http://qt-project.org/), [OpenCV 2.4.7+](http://opencv.org/) and [boost 1.55.0](http://www.boost.org/), it's a cross-platform library.

You need config `INCLUDEPATH` and `LIBS` in `../opensse/opensse.pri` file related your path. 

Now compile it, so easy!


Docker support
==============

Someone still meets compile issue, so I add docker support. Please compare your environment with docker when you meet compile issue.

Run on mac:

```shell
# Install socat and xquartz if you want to run GUI demo.
brew install socat
brew cask install xquartz

# Start XQuartz
open -a XQuartz

# Expose local xquartz socket via socat on a TCP port
socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:\"$DISPLAY\"

# in another window under our opensse repo folder.
docker build -t opensse .  # will takes ~15 minutes to build
docker run -it opensse

# # or use inet ip, like:
# docker run -it -e DISPLAY=$(ipconfig getifaddr en0):0 opensse

# Run command lind demo
docker run -it opensse /bin/bash
cd ~/opensse/build/tools/bin
# You will find all opensse tools

# If you don't want to compile code, You can use zddhub/opensse-demo to run demo:
docker pull zddhub/opensse-demo
docker run -it zddhub/opensse-demo
```

Docker images: [zddhub/opensse-demo](https://hub.docker.com/r/zddhub/opensse-demo/)


Evaluation
==========

You can use dataset and evaluation methodology to evaluate opensse on [SHREC 2012](http://www.itl.nist.gov/iad/vug/sharp/contest/2012/SBR/index.html), [SHREC 2013](http://www.itl.nist.gov/iad/vug/sharp/contest/2013/SBR/index.html), [SHREC 2014](http://www.itl.nist.gov/iad/vug/sharp/contest/2014/SBR/index.html).

You can generate data use `test_search` tool in `tests/test_search`.
```shell
Usage:
test_search -p paramsfile -i searchfilelist -r searchfileroot -o resultdir
```

Troubleshooting
===============
- Could not resolve SDK path for 'macosx10.9'

  There is a workaround:
  Navigate to where you installed Qt (default /Users/your username/Qt) using finder
  Go to the subdirectory `.../5.3/clang_64/mkspecs` directory

  Open the file called qdevice.pri with a text editor
  Change the line !host_build:QMAKE_MAC_SDK = macosx10.9 to:
  * !host_build:QMAKE_MAC_SDK
    = macosx10.10 if you are on OS X 10.10 (Yosemit
  * !host_build:QMAKE_MAC_SDK
    = macosx10.11 if you are on OS X 10.11 (EI Captian))

To do list
==========
- A sketch collect, share and evaluate platform
- A better search framework
- A new rank algorithm

Thanks
======
- [Mathias Eitz](http://cybertron.cg.tu-berlin.de/eitz/)
- [Szymon Rusinkiewicz](http://www.cs.princeton.edu/~smr/)
- [SHREC 2012 - 2014](http://www.itl.nist.gov/iad/vug/sharp/contest/2014/Generic3D/index.html)
- [Tilke Judd](http://people.csail.mit.edu/tjudd/)

License
=======

Code is under the [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Donate
======

I accept tips through [Alipay](http://img.blog.csdn.net/20140506233949640). Scan below qr code to donate:

![Alipay](http://img.blog.csdn.net/20140506233949640 "Donation").

Contact me
==========

If you have any question or idea, please [email to me](mailto:zddhub@gmail.com).
Or search 'zddhub' to find me in social networking platform.
