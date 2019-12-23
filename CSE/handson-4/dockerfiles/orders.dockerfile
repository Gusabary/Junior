FROM weaveworksdemos/orders:0.4.7

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./orders.sh ./orders.sh

USER root
RUN chmod +x ./orders.sh
# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "./orders.sh" ]

