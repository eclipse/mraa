FROM mraa-base

# Update apt-get
RUN apt-get -y update && \
  # Java Build Dependencies
  apt-get -y --no-install-recommends install default-jre default-jdk

# Set JAVA_HOME
ENV JAVA_HOME /usr/lib/jvm/java-8-openjdk-amd64/

# Set Workdir
WORKDIR $MRAA_SRC_DIR

CMD bash
