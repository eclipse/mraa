FROM ubuntu:16.04

ENV DEBIAN_FRONTEND noninteractive

# Update apt-get
RUN apt-get -y update && \
  # Install apt-utils
  apt-get -y --no-install-recommends install apt-utils && \
  # Main Build Dependencies
  apt-get -y --no-install-recommends install git build-essential cmake clang-3.8 g++-4.8 wget libpcre3 libpcre3-dev \
  # Docs Build Dependencies
  python-sphinx doxygen graphviz \
  # Python Build Dependencies
  python python-dev python3 python3-dev \
  # Java Build Dependencies
  default-jre default-jdk \
  # Json Platform Build Dependencies
  pkg-config libjson0 libjson0-dev

# Swig Build Dependencies
RUN wget http://iotdk.intel.com/misc/tr/swig-3.0.10.tar.gz && \
    tar xf swig-3.0.10.tar.gz && cd swig-3.0.10 && \
    ./configure --prefix=/usr/ && make && make install && cd ..

# Node.js Build Dependencies
RUN wget -q -O - https://raw.githubusercontent.com/creationix/nvm/v0.33.2/install.sh | bash

# Set Workdir
WORKDIR /usr/src/app

# Configure Build Arguments
ARG BUILDARCH
ARG BUILDDOC
ARG BUILDSWIG
ARG BUILDSWIGPYTHON
ARG BUILDSWIGNODE
ARG BUILDSWIGJAVA
ARG USBPLAT=OFF
ARG FIRMATA=OFF
ARG ONEWIRE=OFF
ARG JSONPLAT
ARG IMRAA=OFF
ARG FTDI4222=OFF
ARG IPK=OFF
ARG RPM=OFF
ARG ENABLEEXAMPLES
ARG INSTALLGPIOTOOL=OFF
ARG INSTALLTOOLS=OFF
ARG BUILDTESTS
ARG CC
ARG CXX
ARG NODE_VERSION

# Configure Build Environment
ENV NVM_DIR /root/.nvm
ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64/
ENV CC $CC
ENV CXX $CXX
RUN . $NVM_DIR/nvm.sh && nvm install $NODE_VERSION && nvm use $NODE_VERSION && \
    npm install -g node-gyp && node-gyp install

# Copy sources
COPY . .

# Update Submodule
RUN git submodule update --init --recursive

# Fix line ending issue in src/doxy2swig.py and do it executable
RUN tr -d "\r" < src/doxy2swig.py > src/_doxy2swig.py && \
    mv src/_doxy2swig.py src/doxy2swig.py && \
    chmod u+x src/doxy2swig.py

# Change Workdir to build directory
WORKDIR /usr/src/app/build

# Run cmake
RUN . $NVM_DIR/nvm.sh && cmake \
    -DSWIG_EXECUTABLE=/usr/bin/swig \
    -DSWIG_DIR:PATH=/usr/share/swig/3.0.10/ \
    -DBUILDARCH=$BUILDARCH \
    -DBUILDDOC=$BUILDDOC \
    -DBUILDSWIG=$BUILDSWIG \
    -DBUILDSWIGPYTHON=$BUILDSWIGPYTHON \
    -DBUILDSWIGNODE=$BUILDSWIGNODE \
    -DBUILDSWIGJAVA=$BUILDSWIGJAVA \
    -DUSBPLAT=$USBPLAT \
    -DFIRMATA=$FIRMATA \
    -DONEWIRE=$ONEWIRE \
    -DJSONPLAT=$JSONPLAT \
    -DIMRAA=$IMRAA \
    -DFTDI4222=$FTDI4222 \
    -DIPK=$IPK \
    -DRPM=$RPM \
    -DENABLEEXAMPLES=$ENABLEEXAMPLES \
    -DINSTALLGPIOTOOL=$INSTALLGPIOTOOL \
    -DINSTALLTOOLS=$INSTALLTOOLS \
    -DBUILDTESTS=$BUILDTESTS \
    ..

CMD make
