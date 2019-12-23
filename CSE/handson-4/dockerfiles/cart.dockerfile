FROM weaveworksdemos/carts:0.4.8

# copy zookeeper watcher 
COPY ./zk-watcher-1.0-SNAPSHOT.jar ./zkwatcher.jar
COPY ./dependency ./dependency
COPY ./cart.sh ./cart.sh

USER root
RUN chmod +x /usr/src/app/cart.sh

# boost zookeeper watcher
ENTRYPOINT [ "/bin/sh", "-c", "/usr/src/app/cart.sh" ]

