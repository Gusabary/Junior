FROM dplsming/sockshop-user:0.1

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./user.sh ./user.sh

USER root
RUN chmod +x ./user.sh
# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "./user.sh" ]
