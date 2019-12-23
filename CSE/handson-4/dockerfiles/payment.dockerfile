FROM dplsming/sockshop-payment:0.1

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./payment.sh ./payment.sh

USER root
RUN chmod +x ./payment.sh

# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "./payment.sh" ]