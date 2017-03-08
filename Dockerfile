# How to use
# docker build -t opensse .
# docker run -it opensse /bin/bash

FROM zddhub/opencv:3.2.0
MAINTAINER zdd <zddhub@gmail.com>

RUN apt-get update \
	&& apt-get install g++ -y \
	&& apt-get install cmake -y \
	&& apt-get install libboost-all-dev -y

RUN mkdir opensse
COPY . opensse

RUN cd opensse \
	&& mkdir dist && cd dist \
	&& cmake .. \
	&& make \
	&& make install \
	&& ldconfig

CMD [ "sse" ]