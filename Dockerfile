FROM ubuntu:16.04

ENV DEBIAN_FRONTEND noninteractive

# Update apt-get
RUN apt-get -y update && \
  # Install apt-utils
  apt-get -y --no-install-recommends install apt-utils && \
  # Main Build Dependencies
  apt-get -y --no-install-recommends install git build-essential cmake clang-3.8 g++-4.8 wget unzip libpcre3 libpcre3-dev \
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

# Static code analysis scanner
ENV SONAR_DIR /usr/sonar
ENV SONAR_VER "3.0.3.778"
WORKDIR $SONAR_DIR

RUN wget https://sonarsource.bintray.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-${SONAR_VER}-linux.zip && \
    wget https://sonarqube.com/static/cpp/build-wrapper-linux-x86.zip && \
    unzip sonar-scanner-cli-${SONAR_VER}-linux.zip && \
    unzip build-wrapper-linux-x86.zip && \
    rm sonar-scanner-cli-${SONAR_VER}-linux.zip build-wrapper-linux-x86.zip

ENV PATH $SONAR_DIR/sonar-scanner-${SONAR_VER}-linux/bin:$SONAR_DIR/build-wrapper-linux-x86:$PATH

# Set Workdir
ARG MRAA_SRC_DIR
WORKDIR $MRAA_SRC_DIR

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
ARG USEPYTHON3TESTS

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
WORKDIR $MRAA_SRC_DIR/build

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
    -DUSEPYTHON3TESTS=$USEPYTHON3TESTS \
    ..

CMD make
