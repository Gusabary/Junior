FROM dplsming/sockshop-catalogue:0.1

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./catalogue.sh ./catalogue.sh

USER root
# WORKDIR /
RUN chmod +x /catalogue.sh

# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "/catalogue.sh" ]
