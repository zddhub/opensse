FROM ubuntu:latest
MAINTAINER zdd <zddhub@gmail.com>

# shut up debconf messages (https://github.com/phusion/baseimage-docker/issues/58)
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

RUN apt-get update && apt-get install software-properties-common -y --fix-missing

RUN add-apt-repository --yes ppa:ubuntu-sdk-team/ppa \
	&& add-apt-repository --yes ppa:lttng/daily \
	&& add-apt-repository ppa:amarburg/opencv3 -y \
	&& apt-get update -qq \
	&& apt-get install -qq libopencv3-dev \
	&& sudo apt-get install -qq libboost-all-dev

RUN mkdir /root/opensse
COPY . /root/opensse

RUN cd /root/opensse \
	&& mkdir release && cd release \
	&& cmake .. \
	&& make \
	&& make install

CMD [ "sse" ]
