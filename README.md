OpenSSE: Open Sketch Search Engine [![Build Status](https://travis-ci.org/zddhub/opensse.svg?branch=zdd)](https://travis-ci.org/zddhub/opensse)
==================================

[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/zddhub/opensse?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Hello everybody!

I'm very interested in [Mathias Eitz's works](http://cybertron.cg.tu-berlin.de/eitz/projects/sbsr/) on SIGGRAPH 2012, so I developed OpenSSE, Just for fun.

This is an open source sketch search engine for 3D object retrieval based on sketch image as input. In fact, it can retrieve more, such as images, videos, point cloud models and volume models, you just need get their line drawing images than I know you can.

In order to clearly show how to work, I used single thread and ascii encoding for file storage.

Zhang Dongdong


Demo powered by OpenSSE
=======================

* Open Sketch Search

This demo deals with 3D mesh models. See more search results, click [demo videos](http://opensse.com).

![Sketch search demo](data/opensse.gif "Sketch search demo")

* Open Sketch Recognize

Recognize input sketch image, you can try it via [web version](http://online.opensse.com/). See more search results, click [demo videos](http://sr.opensse.com/).

![Sketch recognize demo](data/opensr.gif "Sketch recognize demo")


Interesting demo
================

*Note:* The below demos are *NOT* supported by OpenSSE, but I think there are in the same field. And I am very interested them, so list here:

- [AutoDraw](https://www.autodraw.com/)
- [QuickDraw](https://quickdraw.withgoogle.com/)


How to install
==============

After installed OpenSSE's dependency - [OpenCV 3.0.0+](http://opencv.org/), You can compile and install it like below:

```sh
mkdir release && cd release
cmake ..
make
make install
```


OpenSSE Wiki
============
More detail info, go to [OpenSSE Wiki](https://github.com/zddhub/opensse/wiki)


Todo List
=========
- [ ] Recognize sketch
- [ ] Speed up train steps
- [ ] Support big image set


Thanks
======
- [Mathias Eitz](http://cybertron.cg.tu-berlin.de/eitz/)
- [Szymon Rusinkiewicz](http://www.cs.princeton.edu/~smr/)
- [SHREC 2012 - 2014](http://www.itl.nist.gov/iad/vug/sharp/contest/2014/Generic3D/index.html)
- [Tilke Judd](http://people.csail.mit.edu/tjudd/)


License
=======

[Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0)


Contact me
==========

If you have any question or idea, please [email to me](mailto:zddhub@gmail.com).
