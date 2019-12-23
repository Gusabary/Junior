FROM weaveworksdemos/shipping:0.4.8

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./shipping.sh ./shipping.sh

USER root
RUN chmod +x ./shipping.sh

# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "./shipping.sh" ]
