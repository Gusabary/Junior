FROM mongo:3.4

ADD ./jdk-8u231-linux-x64.tar.gz /usr/local/
ENV JAVA_HOME /usr/local/jdk1.8.0_231/
ENV PATH $JAVA_HOME/bin:$PATH

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./mongo.sh ./mongo.sh

USER root
RUN chmod +x /mongo.sh

ENTRYPOINT [ "/bin/sh", "-c", "/mongo.sh" ]
