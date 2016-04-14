# Run opensse in a container
# More info, please visit https://github.com/zddhub/opensse

# # Install socat and xquartz if you want to run GUI demo.
# brew install socat
# brew cask install xquartz
#
# # Start XQuartz
# open -a XQuartz
#
# # Expose local xquartz socket via socat on a TCP port
# socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:\"$DISPLAY\"
#
# # in another window under our opensse repo folder.
# docker build -t opensse .  # will takes ~15 minutes to build
# docker run -it opensse
#
# # or use inet ip, like:
# # docker run -it -e DISPLAY=$(ipconfig getifaddr en0):0 opensse
#
# # Run command lind demo
# docker run -it opensse /bin/bash
# cd ~/opensse/build/tools/bin
# # You will find all opensse tools

FROM zddhub/opensse-env:latest
MAINTAINER zdd <zddhub@gmail.com>

RUN mkdir /root/opensse
COPY . /root/opensse

RUN cd /root/opensse \
  && mkdir build \
  && cd build \
  && /opt/Qt5.3.1/5.3/gcc_64/bin/qmake ../opensse.pro \
  && make

CMD [ "/root/opensse/build/gui/bin/SketchSearchDemo" ]
